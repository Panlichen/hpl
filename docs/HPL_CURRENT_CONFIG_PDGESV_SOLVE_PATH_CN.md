# 当前 HPL.dat 首选配置下的分解与求解执行路径

本文结合 `testing/ptest/HPL.dat`、`docs/HPL_FUNCTION_CALL_FLOW_BY_CONFIG.md` 和相关源码，说明当前首选配置下 HPL 从矩阵进入求解器后，如何完成 LU 分解、尾随矩阵更新和最终上三角回代。

这里的“首选配置”沿用 `docs/HPL_FUNCTION_CALL_FLOW_BY_CONFIG.md` 的规则：当 `HPL.dat` 中某项给出多个候选值时，选第一个值继续分析。

## 1. 当前采用的配置

来自 `testing/ptest/HPL.dat` 的第一组配置如下：

| 配置项 | 首选值 | 对执行路径的影响 |
| --- | --- | --- |
| `N` | `29` | 全局问题规模，求解 `29 x 29` 线性系统 |
| `NB` | `1` | 每个 panel 宽度为 1，主循环逐列推进 |
| `P x Q` | `2 x 2` | 二维进程网格，既有进程行通信也有进程列通信 |
| `PFACT` | `0` | 非递归 panel 分解选择 left-looking |
| `NBMIN` | `2` | 递归 panel 分解到宽度小于等于 2 时停止 |
| `NDIV` | `2` | 递归 panel 每层二分 |
| `RFACT` | `0` | 递归 panel 分解选择 left-looking |
| `BCAST` | `0` | panel 广播拓扑为 `1-ring` |
| `DEPTH` | `0` | 不启用 look-ahead，走 `HPL_pdgesv0` |
| `SWAP` | `2` | 混合交换策略 |
| `swapping threshold` | `64` | 混合交换阈值 |
| `L1 in` | `0` | L1 使用 transposed 存储 |
| `U in` | `0` | U 使用 transposed 存储 |
| `Equilibration` | `1` | 开启平衡处理 |
| `alignment` | `8` | 动态缓冲区按 8 个 double 对齐 |

由这些配置得到的关键函数指针是：

| 函数指针/参数 | 实际值 |
| --- | --- |
| `algo.depth` | `0` |
| `algo.btopo` | `HPL_1RING` |
| `algo.pffun` | `HPL_pdpanllT` |
| `algo.rffun` | `HPL_pdrpanllT` |
| `algo.upfun` | `HPL_pdupdateTT` |
| `algo.fswap` | `HPL_SW_MIX` |
| `algo.fsthr` | `64` |

其中 `T` 表示对应的局部块采用 transposed 存储变体。

## 2. 从测试函数到求解器

在 `testing/ptest/HPL_pdtest.c` 中，矩阵 `[A | b]` 已经按二维块循环方式分布并生成好，然后调用：

```text
HPL_pdgesv(GRID, ALGO, &mat)
```

`HPL_pdgesv` 是总求解入口。它先设置：

```text
A->info = 0
```

然后根据 look-ahead 深度选择 LU 主调度：

```text
if ALGO->depth == 0 or GRID->npcol == 1:
    HPL_pdgesv0(GRID, ALGO, A)
else:
    HPL_pdgesvK2(GRID, ALGO, A)
```

当前配置中：

```text
ALGO->depth = 0
GRID->npcol = 2
```

因此一定进入：

```text
HPL_pdgesv
  -> HPL_pdgesv0
```

LU 分解结束后，如果 `A->info == 0`，再调用：

```text
HPL_pdtrsv(GRID, A)
```

所以当前求解主线是：

```text
HPL_pdgesv
  -> HPL_pdgesv0      // 不带 look-ahead 的 right-looking LU
  -> HPL_pdtrsv       // 上三角回代
```

## 3. HPL_pdgesv0 的角色

`src/pgesv/HPL_pdgesv0.c` 实现的是不带 look-ahead 的 right-looking LU 主循环。

它的特点是四个阶段严格串行：

```text
初始化当前 panel
  -> 分解当前 panel
  -> 广播当前 panel
  -> 更新尾随矩阵
```

由于当前 `NB=1`，全局 `N=29`，所以主循环执行 29 轮：

```text
j = 0, 1, 2, ..., 28
```

每轮：

```text
n  = N - j
jb = min(n, NB) = 1
```

也就是说，每轮只处理一列 panel。

## 4. panel 创建和重建

`HPL_pdgesv0` 首先分配一个长度为 1 的 panel 指针数组：

```text
panel = malloc(sizeof(HPL_T_panel *))
```

然后创建第一个 panel：

```text
HPL_pdpanel_new(GRID, ALGO, N, N+1, min(N, nb), A, 0, 0, tag, &panel[0])
```

因为 `DEPTH=0`，没有多个 panel 同时处于流水线中，所以只需要 `panel[0]` 一个描述符。

进入每轮循环后，先做：

```text
HPL_pdpanel_free(panel[0])
HPL_pdpanel_init(GRID, ALGO, n, n+1, jb, A, j, j, tag, panel[0])
```

这两步的含义是：

1. 清理上一轮 panel 的临时状态。
2. 把同一个 `panel[0]` 重新绑定到当前全局位置 `(j, j)`。
3. 设置当前尾随子矩阵大小为 `n x (n+1)`。
4. 设置当前 panel 宽度 `jb=1`。
5. 计算当前 panel 所属的进程行/列、本地指针、L1/L2/U/主元数组等工作区。

因此，`panel[0]` 不是单纯的“几列矩阵”，而是当前面板分解、广播、交换、更新共同使用的状态包。

## 5. 当前 panel 分解

每轮 panel 初始化后，`HPL_pdgesv0` 调用：

```text
HPL_pdfact(panel[0])
```

`HPL_pdfact` 是 panel 分解统一入口。它会先把 panel 描述符向前推进：

```text
jb = PANEL->jb
PANEL->n  -= jb
PANEL->ja += jb
```

然后判断当前进程列是否拥有这个 panel：

```text
if mycol != PANEL->pcol:
    return
```

也就是说，只有当前 panel 所属的进程列真正执行分解；其他进程列等待后续沿进程行广播得到结果。

接着它调用递归分解函数指针：

```text
PANEL->algo->rffun(...)
```

当前配置下：

```text
rffun = HPL_pdrpanllT
```

## 6. 为什么递归分解会立即退化为非递归分解

`HPL_pdrpanllT` 是 recursive left-looking + transposed L1 的 panel 分解核。它入口处有停止条件：

```text
if N <= PANEL->algo->nbmin:
    PANEL->algo->pffun(...)
    return
```

当前配置：

```text
N     = jb = 1
NBMIN = 2
```

因此每一轮都会满足：

```text
1 <= 2
```

实际执行路径变成：

```text
HPL_pdfact
  -> HPL_pdrpanllT
     -> HPL_pdpanllT
```

这点很关键：配置上选择了递归分解核，但由于 `NB=1` 小于递归停止阈值，真正做面板分解工作的是非递归 `HPL_pdpanllT`。

## 7. HPL_pdpanllT 在 NB=1 下做什么

`HPL_pdpanllT` 是 left-looking + transposed L1 的非递归 panel 分解核。

在当前 `jb=1` 的场景下，它每次只处理一列，主要工作可以理解为：

1. 在当前进程列的本地片段中找主元候选。
2. 在进程列内部比较主元候选，确定全局主元。
3. 执行必要的行交换。
4. 把主元行相关数据传播给同一进程列中的其他进程。
5. 对当前列主元下方元素做缩放，形成 L 的一列。

对应的重要函数包括：

```text
HPL_dlocmax     // 本地找最大绝对值主元候选
HPL_pdmxswp     // 进程列内主元竞争、行交换、主元行传播
HPL_dlocswpT    // transposed 存储形式下的本地行交换
HPL_dscal       // 缩放当前列，形成 L 的倍率
```

由于当前 panel 宽度只有 1，很多原本处理 panel 内多个列之间相互更新的 BLAS 操作会退化或变得非常小。

## 8. panel 广播：1-ring 路径

当前 panel 分解完成后，`HPL_pdgesv0` 调用：

```text
HPL_binit(panel[0])
do
    HPL_bcast(panel[0], &test)
while test != HPL_SUCCESS
HPL_bwait(panel[0])
```

当前 `BCAST=0`，所以广播拓扑为：

```text
HPL_1RING
```

对应路径是：

```text
HPL_binit
  -> HPL_binit_1ring

HPL_bcast
  -> HPL_bcast_1ring

HPL_bwait
  -> HPL_bwait_1ring
```

这个广播语义上是在进程行内广播当前 panel 的分解结果，但实现上不是直接调用 `MPI_Bcast`。HPL 使用可调拓扑，1-ring 中每个进程从前驱接收，再发送给后继，底层主要是点对点 MPI。

广播传递的数据包括：

```text
L1 / L2 相关块
主元信息 DPIV / DINFO
后续更新需要的 panel 载荷
```

在 `DEPTH=0` 下，这个广播必须完成后才进入尾随矩阵更新，没有和下一轮 panel 分解重叠。

## 9. 尾随矩阵更新：HPL_pdupdateTT

当前配置：

```text
L1 in = transposed
U  in = transposed
```

因此 `HPL_pddriver.c` 选择：

```text
algo.upfun = HPL_pdupdateTT
```

在 `HPL_pdgesv0` 中实际调用为：

```text
HPL_pdupdate(NULL, NULL, panel[0], -1)
```

这里 `PBCST=NULL`、`IFLAG=NULL` 表示没有 look-ahead 场景中的“下一个 panel 广播”需要同步推进；当前函数只负责根据已经广播完成的 `panel[0]` 更新尾随矩阵。

`HPL_pdupdateTT` 的核心任务是：

1. 读取 panel 分解产生的主元信息。
2. 应用行交换。
3. 构造或重分发行面板 `U`。
4. 对右下方尾随矩阵执行 BLAS 更新。

在当前配置下，swap 路径还会继续判断：

```text
fswap = HPL_SW_MIX
fsthr = 64
```

混合交换策略通常在问题足够大时走一种路径，小问题时走另一种路径。当前 `N=29`，每轮待更新规模不会超过 64，因此实际会落到：

```text
HPL_pdlaswp00T
```

随后典型计算包括：

```text
HPL_dtrsm   // 使用 L1/U 局部块做三角求解，形成更新所需的 U
HPL_dgemm   // 用 L2 和 U 更新尾随矩阵
HPL_dlatcpy // 必要时把 transposed 缓冲区内容写回矩阵区域
```

所以每轮 right-looking 更新的数学含义是：

```text
A_trailing = A_trailing - L_panel * U_panel
```

只不过在分布式环境里，`L_panel`、`U_panel`、pivot 和尾随矩阵块分散在不同进程上，需要先经过分解、交换和行广播把必要数据放到各自本地。

## 10. 消息 tag 如何推进

每轮更新结束后：

```text
tag = MNxtMgid(tag, MSGID_BEGIN_FACT, MSGID_END_FACT)
```

`tag` 是 panel 分解/广播相关通信的消息编号。它在一个安全范围内循环递增，避免不同 panel 的点对点消息在 MPI 层被误匹配。

由于当前 `DEPTH=0`，同一时间只有一个 panel 在处理，tag 压力较小；但 HPL 仍然保留这套滚动机制，和 look-ahead 路径保持一致的消息管理方式。

## 11. LU 分解结束后的矩阵状态

`HPL_pdgesv0` 完成所有 29 轮后，释放：

```text
HPL_pdpanel_disp(&panel[0])
free(panel)
```

此时分布式矩阵中已经原地保存了 LU 分解结果：

```text
[A | b] -> [[L, U] | y]
```

这里的 `b` 在分解过程中已经被对应的行交换和消元操作更新成 `y`。下三角因子 `L` 与上三角因子 `U` 按 HPL 的分布式本地布局保留在原矩阵缓冲区和 panel 临时布局所约定的位置。

随后控制流回到 `HPL_pdgesv`。

## 12. 上三角回代：HPL_pdtrsv

如果分解没有失败：

```text
if A->info == 0:
    HPL_pdtrsv(GRID, A)
```

`HPL_pdtrsv` 解的是：

```text
U x = y
```

其中 `y` 是 LU 分解阶段已经同步更新过的右端项。

`HPL_pdtrsv` 的核心特征是从右下角开始按块回代。它要解决两个分布式问题：

1. 右端项 `b/y` 最初作为 `[A | b]` 的额外列，可能不在最后一个对角块所在的进程列。
2. 每解出一个解块，都要把它传播给需要更新左侧未知量的进程。

因此它先执行一次右端项搬运：

```text
if current rhs owner column != last diagonal owner column:
    HPL_send / HPL_recv
```

然后按对角块从右向左推进：

```text
for diagonal blocks from right to left:
    传播上一轮解块
    用 HPL_dgemv 计算局部部分更新
    把部分更新发给当前拥有列
    累加更新到当前 rhs
    在对角块拥有者上调用 HPL_dtrsv
    保存当前解块
```

最后用广播把必要的解向量片段复制到进程行中，使 `A->X` 中保存本地解向量片段。

## 13. 当前配置下的完整调用链

综合起来，当前首选配置下从求解器开始的主路径是：

```text
HPL_pdgesv
  -> HPL_pdgesv0
     -> HPL_pdpanel_new
     -> loop j = 0..28
        -> HPL_pdpanel_free
        -> HPL_pdpanel_init
        -> HPL_pdfact
           -> HPL_pdrpanllT
              -> HPL_pdpanllT        // 因 NB=1 <= NBMIN=2
                 -> HPL_dlocmax
                 -> HPL_pdmxswp
                 -> HPL_dlocswpT
                 -> HPL_dscal
        -> HPL_binit
           -> HPL_binit_1ring
        -> HPL_bcast
           -> HPL_bcast_1ring
        -> HPL_bwait
           -> HPL_bwait_1ring
        -> HPL_pdupdateTT
           -> HPL_pdlaswp00T
           -> HPL_dtrsm
           -> HPL_dgemm
           -> HPL_dlatcpy
        -> MNxtMgid
     -> HPL_pdpanel_disp
  -> HPL_pdtrsv
     -> HPL_send / HPL_recv
     -> HPL_dgemv
     -> HPL_daxpy
     -> HPL_dtrsv
     -> HPL_dcopy
     -> HPL_broadcast
```

## 14. 当前路径的执行特征

这条路径很适合学习 HPL 的基本机制，因为：

1. `DEPTH=0`，没有 look-ahead，panel 分解、广播、更新顺序清晰。
2. `NB=1`，每轮只处理一列，递归 panel 分解会直接退化到非递归核。
3. `BCAST=1ring`，panel 广播的通信路径相对直观。
4. `L1/U` 都是 transposed，因此 update 路径固定为 `HPL_pdupdateTT`。

但它不是高性能配置。`NB=1` 会导致 panel 过窄，Level 3 BLAS 利用率低，通信频率高。真实跑分通常会选择更大的 `NB` 和可能大于 0 的 look-ahead 深度，以增加计算/通信重叠并提高矩阵更新阶段的效率。
