# HPL 函数级调用流程梳理（按 `testing/ptest/HPL.dat` 首选配置）

## 1. 文档目标

本文从 `testing/ptest/HPL_pddriver.c` 出发，结合：

- `docs/HPL_IMPLEMENTATION_GUIDE.md`
- `docs/WWW_READING_GUIDE_AND_EXAM.md`

沿着一条**具体、唯一**的配置路径，梳理 HPL 在本项目中的函数级调用流程。

本文不再泛泛枚举全部可选实现，而是遵循以下规则：

1. 如果 `testing/ptest/HPL.dat` 给出了多个候选值，则选择**第一个**候选值继续分析。
2. 如果某个配置项没有显式指定，则也选择它的**第一个默认/候选分支**。
3. 如果某个高层配置虽然选中了某条路径，但运行时又因为参数规模触发了更细的基准条件，那么以**真实执行到的函数**为准继续追踪。

## 2. 本文采用的配置路径

`testing/ptest/HPL.dat` 中本次分析使用的配置为：

| 配置项 | 取值 | 说明 |
| --- | --- | --- |
| `N` | `29` | 取第一个问题规模 |
| `NB` | `1` | 取第一个块大小 |
| `PMAP` | `0` | Row-major |
| `P x Q` | `2 x 2` | 取第一组进程网格 |
| `threshold` | `16.0` | 残差阈值 |
| `PFACT` | `0` | `HPL_LEFT_LOOKING` |
| `NBMIN` | `2` | 递归停止阈值 |
| `NDIV` | `2` | 递归时每层分成 2 个子块 |
| `RFACT` | `0` | `HPL_LEFT_LOOKING` |
| `BCAST` | `0` | `HPL_1RING` |
| `DEPTH` | `0` | 无 look-ahead |
| `SWAP` | `2` | `HPL_SW_MIX` |
| `swapping threshold` | `64` | 混合交换阈值 |
| `L1 in` | `0` | `transposed` |
| `U in` | `0` | `transposed` |
| `Equilibration` | `1` | 开启 |
| `alignment` | `8` | 双精度对齐粒度 |

## 3. 由配置推导出的函数选择

### 3.1 在 `HPL_pddriver.c` 中解出来的函数指针

根据 `L1notran = 0`、`Unotran = 0`、`PFACT = left`、`RFACT = left`、`DEPTH = 0`、`BCAST = 1ring`，最终得到：

- `algo.pfact = HPL_LEFT_LOOKING`
- `algo.pffun = HPL_pdpanllT`
- `algo.rfact = HPL_LEFT_LOOKING`
- `algo.rffun = HPL_pdrpanllT`
- `algo.upfun = HPL_pdupdateTT`
- `algo.btopo = HPL_1RING`
- `algo.depth = 0`
- `algo.fswap = HPL_SW_MIX`
- `algo.fsthr = 64`

这意味着高层调用主线会选择：

```text
HPL_pdgesv
  -> HPL_pdgesv0          (因为 DEPTH = 0)
  -> HPL_pdfact
     -> HPL_pdrpanllT     (递归 left-looking, transpose 存储)
        -> HPL_pdpanllT   (运行时满足基准条件，直接落到非递归核)
  -> HPL_binit / HPL_bcast / HPL_bwait
     -> HPL_binit_1ring / HPL_bcast_1ring / HPL_bwait_1ring
  -> HPL_pdupdateTT
     -> HPL_pdlaswp00T    (因为 SWAP=mix 且当前 n <= 64)
  -> HPL_pdtrsv
```

### 3.2 一个容易忽略但很重要的“运行时分支”

虽然配置层面 `RFACT=left` 选择了递归面板分解 `HPL_pdrpanllT`，但本次配置里：

- `NB = 1`
- 当前 panel 宽度 `jb = min(remaining, NB) = 1`
- `NBMIN = 2`

因此在 `HPL_pdrpanllT(PANEL, M, N=1, ICOFF, WORK)` 入口就满足：

```c
if( N <= PANEL->algo->nbmin ) { PANEL->algo->pffun(...); return; }
```

所以**真实执行路径**并不会展开递归，而是立即转入：

- `HPL_pdpanllT`

也就是说，这条配置虽然“名义上选中了递归核”，但**在实际规模下真正完成面板分解工作的，是非递归的 `HPL_pdpanllT`**。

## 4. 从 `main` 开始的函数级调用主线

## 4.1 `testing/ptest/HPL_pddriver.c:main`

这是整个测试驱动入口。

### 主要步骤

1. `MPI_Init`
2. `MPI_Comm_rank / MPI_Comm_size`
3. `HPL_pdinfo(...)`
4. 多重循环枚举 `HPL.dat` 中的参数组合
5. 对每个 `P x Q` 调用 `HPL_grid_init`
6. 根据 `L1notran / Unotran / PFACT / RFACT / BCAST / DEPTH / SWAP` 组装 `algo`
7. 调用 `HPL_pdtest(&test, &grid, &algo, N, NB)`
8. 结束后打印总测试统计并 `MPI_Finalize`

### 本次配置下真正选中的第一条组合

因为每个列表都取第一个值，所以第一次真正执行的测试组合就是：

```text
N=29, NB=1, P=2, Q=2, DEPTH=0, BCAST=1ring,
PFACT=left, RFACT=left, NBMIN=2, NDIV=2,
SWAP=mix, threshold=64, L1=transposed, U=transposed
```

这一步的核心作用是：

- 把**文本配置**翻译成 `HPL_T_palg`
- 把**算法选择**翻译成函数指针
- 把**一次实验**委托给 `HPL_pdtest`

## 4.2 `testing/ptest/HPL_pdtest.c:HPL_pdtest`

这是“单次测试”的执行函数。

### 它负责什么

1. 根据 `GRID`、`N`、`NB` 计算本地矩阵尺寸
2. 分配 `[A | b]` 和解向量 `x` 的本地存储
3. 生成测试矩阵 `HPL_pdmatgen`
4. 调用真正的求解器 `HPL_pdgesv`
5. 汇总计时并打印性能
6. 重新生成原始矩阵，计算残差并做正确性校验

### 函数级调用链

```text
HPL_pdtest
  -> HPL_grid_info
  -> HPL_numroc                 // 计算本地行/列规模
  -> HPL_all_reduce(HPL_max)    // 检查全局是否有进程分配失败
  -> HPL_pdmatgen               // 生成 [A | b]
  -> HPL_barrier
  -> HPL_pdgesv                 // 真正求解
  -> HPL_ptimer_combine         // 汇总计时
  -> HPL_pdmatgen               // 重新生成原矩阵做校验
  -> HPL_pdlange                // 计算 ||A||、||x||
  -> HPL_all_reduce(HPL_max)    // 在拥有 b 的进程列上求 BnormI
  -> HPL_broadcast              // 沿行复制 BnormI
  -> HPL_dgemv                  // 计算 b - A x
  -> HPL_reduce(HPL_sum)        // 把残差规约到进程列 0
  -> HPL_pdlange                // 计算 ||b-Ax||_oo
```

### 本函数中的通信重点

- `HPL_barrier(GRID->all_comm)`
  - 集合语义
  - 用于统一计时起点
- `HPL_all_reduce(..., HPL_max, GRID->col_comm)`
  - 集合语义
  - 算子：`HPL_max`
  - 用来找 `BnormI`
- `HPL_broadcast(..., GRID->row_comm)`
  - 集合语义
  - 让所有进程都拿到 `BnormI`
- `HPL_reduce(..., HPL_sum, root=0, GRID->row_comm)`
  - 集合语义
  - 算子：`HPL_sum`
  - 目的：把分布式残差累加到根列

## 4.3 `src/pgesv/HPL_pdgesv.c:HPL_pdgesv`

这是总求解入口。

### 本次配置下的分支选择

因为：

- `ALGO->depth = 0`
- `GRID->npcol = 2`

所以这里走：

```text
HPL_pdgesv
  -> HPL_pdgesv0
  -> HPL_pdtrsv
```

不会走 `HPL_pdgesvK2`。

### 它的角色

- 负责选择 LU 主调度方式
- 在 LU 分解完成后统一进入回代 `HPL_pdtrsv`

## 4.4 `src/pgesv/HPL_pdgesv0.c:HPL_pdgesv0`

这是**无 look-ahead** 的 right-looking LU 主循环。

### 每轮迭代的四个阶段

对每个全局列偏移 `j = 0, 1, 2, ...`（因为 `NB=1`）：

1. 重新构造当前 panel 描述符
2. 分解当前 panel
3. 广播当前 panel
4. 更新尾随矩阵

### 函数级调用链

```text
HPL_pdgesv0
  -> HPL_pdpanel_new           // 只在第一次创建 panel[0]
  loop over j
    -> HPL_pdpanel_free
    -> HPL_pdpanel_init
    -> HPL_pdfact
    -> HPL_binit
    -> HPL_bcast               // 反复轮询直到 HPL_SUCCESS
    -> HPL_bwait
    -> HPL_pdupdateTT          // 因为 L1/U 都是 transposed
```

### 为什么这里一定是串行四阶段

因为 `DEPTH=0`，所以：

- 当前 panel 的分解、广播、更新不会和下一个 panel 重叠
- 更适合做“主线理解”
- 但性能上不如 look-ahead 路径

## 4.5 `src/panel/HPL_pdpanel_init.c:HPL_pdpanel_init`

这是 panel 从“概念”落到“数据结构”的关键一步。

### 它做了什么

1. 计算当前 `(IA, JA)` 对应的本地偏移 `ii/jj`
2. 计算当前尾随矩阵在本地的尺寸 `mp/nq`
3. 设置 `PANEL->A / PANEL->jb / PANEL->m / PANEL->n / PANEL->prow / PANEL->pcol`
4. 一次性分配并布局：
   - `L2`
   - `L1`
   - `DPIV`
   - `DINFO`
   - `U`
   - `IWORK`
5. 为后续面板广播准备连续或可打包的通信载荷

### 这里为什么重要

因为 HPL 的 panel 不只是“几列矩阵”：

- 它还是后续广播、交换、更新要携带的**状态包**
- `L2 + L1 + DPIV + DINFO (+ U workspace)` 的布局，直接决定后续通信效率

## 4.6 `src/pfact/HPL_pdfact.c:HPL_pdfact`

这是“当前 panel 分解”的统一入口。

### 本次配置下的真实调用链

```text
HPL_pdfact
  -> algo.rffun = HPL_pdrpanllT
     -> (N=1 <= NBMIN=2)
     -> algo.pffun = HPL_pdpanllT
```

### 关键点

- 只有拥有当前 panel 的**进程列**真正执行这一步
- 其他进程列不做分解，等后续广播拿结果
- 这一步是整个算法关键路径上的核心瓶颈之一

### 通信性质

`HPL_pdfact` 自己只是入口，但它内部选中的 panel 核会触发：

- 主元搜索
- 主元行交换
- 主元行传播

这些通信都主要发生在**panel 所属的进程列内部**

## 4.7 `src/pfact/HPL_pdrpanllT.c:HPL_pdrpanllT`

这是“递归 left-looking + transposed 存储”的面板分解核。

### 但本次配置下它没有真正展开递归

入口参数是：

- `N = jb = 1`
- `NBMIN = 2`

所以立即命中：

```text
if( N <= nbmin ) -> HPL_pdpanllT(...)
```

也就是说：

- **配置上选中了递归核**
- **执行上直接退化为非递归核**

这点在分析 HPL 时非常重要，不能只看 `RFACT` 文本配置。

## 4.8 `src/pfact/HPL_pdpanllT.c:HPL_pdpanllT`

这是真正执行当前 panel 分解的函数。

### 本次配置下，因为 `NB=1`

当前 panel 只有 1 列，因此内部行为会进一步简化：

1. `HPL_dlocmax`
   - 在本地列片段中找绝对值最大元
2. `HPL_pdmxswp`
   - 在进程列内部融合执行：
     - 主元竞争
     - 行交换
     - 主元行传播
3. `HPL_dlocswpT`
   - 完成本地交换
4. 若需要，对当前列执行缩放

因为 `Nm1 = N - 1 = 0`，所以按列推进的主循环几乎不会展开。

### 这里的通信重点

`HPL_pdmxswp` 是 panel 分解路径里最关键的通信点之一：

- 通信语义上类似“主元归约 + 主元行传播”
- 实现上主要还是**点对点通信**
- 这样做是为了把：
  - 主元搜索
  - 行交换
  - 主元行广播
  
  融合成一个更紧凑的通信步骤

## 4.9 `src/comm/HPL_binit.c` / `src/comm/HPL_1ring.c`

当前配置的面板广播拓扑是：

- `BCAST = 0`
- 对应 `HPL_1RING`

因此广播路径为：

```text
HPL_binit
  -> HPL_binit_1ring
HPL_bcast
  -> HPL_bcast_1ring
HPL_bwait
  -> HPL_bwait_1ring
```

### 具体做了什么

#### `HPL_binit_1ring`

- 必要时调用 `HPL_copyL`
- 调用 `HPL_packL`
- 构造当前 panel 广播要用的数据类型/缓冲区

#### `HPL_bcast_1ring`

按逻辑环传播 panel：

- 根进程先发给下一个 rank
- 非根进程用 `MPI_Iprobe` 轮询前驱
- 收到后：
  - 先 `MPI_Recv`
  - 如果自己不是最后一个，再 `MPI_Send` 给后继
- 若尚未收到，则返回 `HPL_KEEP_TESTING`

#### `HPL_bwait_1ring`

- 释放 `MPI_Datatype`

### 通信类型

- 语义上：广播
- 实现上：**纯点对点**
  - `MPI_Send`
  - `MPI_Iprobe`
  - `MPI_Recv`

### 为什么这样做

因为 HPL 希望把“广播语义”落到一组可调拓扑上：

- 1-ring
- 1-ring modified
- 2-ring
- long

这样可以根据机器网络特性调优，而不是被固定的 MPI_Bcast 实现绑定。

## 4.10 `src/pgesv/HPL_pdupdateTT.c:HPL_pdupdateTT`

这一步负责：

1. 消化 panel 分解产生的主元信息
2. 构造/传播行面板 `U`
3. 更新尾随子矩阵

### 为什么是 `TT`

因为本次配置中：

- `L1notran = 0`
- `Unotran = 0`

所以选择：

- `HPL_pdupdateTT`

### 本次配置下会落到哪条 swap 路径

函数里会判断：

```text
if( fswap == HPL_SWAP01 ) -> HPL_pdlaswp01T
else if( fswap == HPL_SW_MIX && n > tswap ) -> HPL_pdlaswp01T
else -> HPL_pdlaswp00T
```

本次配置：

- `fswap = HPL_SW_MIX`
- `tswap = 64`
- `N = 29`
- 本地 `n` 不会大于 64

所以会实际走：

- `HPL_pdlaswp00T`

而**不会**走 `HPL_pdlaswp01T`。

### 函数级调用链

```text
HPL_pdupdateTT
  -> HPL_bcast(PBCST, &test)        // 推进下一 panel 的广播进度
  -> HPL_pdlaswp00T                 // 本次配置下选中的 swap/U 路径
  -> HPL_dtrsm                      // 冗余求解 U
  -> HPL_dgemm                      // 用 L2 和 U 更新尾随矩阵
  -> HPL_dlatcpy                    // 需要时把 U 写回 A 的顶部
  -> 反复 HPL_bcast(PBCST, &test)   // 边算边轮询广播是否完成
```

### 通信重点

这里是整条路径里通信最复杂的部分之一：

- `PBCST` 的 panel 广播
  - 语义：广播
  - 实现：点对点环形广播
- `HPL_pdlaswp00T`
  - 语义：行交换 + U 重分发
  - 实现：主要是点对点
- 没有使用标准 `MPI_Bcast` 一把做完
  - 因为这里不仅要传数据，还要配合 pivot 布局、U 的形成与本地 BLAS 更新节奏

## 4.11 `src/pgesv/HPL_pdtrsv.c:HPL_pdtrsv`

LU 分解完成后，统一进入回代。

### 它做了什么

1. 把右端项 `b` 挪到拥有最后对角块的进程列
2. 从右下角对角块开始，按块做回代
3. 在列通信器里传播上一轮已经求出的解块
4. 在行通信器里发送部分更新
5. 当前对角块拥有者做本地 `HPL_dtrsv`
6. 最后把最后一个解块复制到所有进程行

### 函数级调用链

```text
HPL_pdtrsv
  -> HPL_send / HPL_recv            // 初始搬运 rhs
  loop over diagonal blocks from right to left
    -> HPL_send / HPL_recv          // 传播上一轮解块
    -> HPL_dgemv                    // 计算部分更新
    -> HPL_send / HPL_recv          // 把部分更新发到当前拥有列
    -> HPL_daxpy                    // 累加收到的部分更新
    -> HPL_dtrsv                    // 求当前对角块
    -> HPL_dcopy                    // 写回 XR
  -> HPL_broadcast                  // 复制最后一个解块
```

### 通信性质

- 绝大多数步骤：**点对点**
  - `HPL_send`
  - `HPL_recv`
- 最后一步 `HPL_broadcast`
  - 语义：广播
  - 作用：把最后一个解块复制到所有相关进程

### 为什么回代也不直接用标准集合通信

因为回代本身是一个强依赖顺序的低时延流水线：

- 每个对角块都依赖右边的已解块
- 需要边传播边更新
- 小块消息很多，时延比带宽更关键

所以 HPL 采用了按行/按列交错的点对点流水方式，而不是一次性的大集合操作。

## 5. 整条路径的“真实执行序列”

按照本次 `HPL.dat` 首选配置，函数级真实主线可以压缩为：

```text
main
  -> HPL_pdinfo
  -> HPL_grid_init
  -> HPL_pdtest
     -> HPL_pdmatgen
     -> HPL_barrier
     -> HPL_pdgesv
        -> HPL_pdgesv0
           loop j = 0 .. N-1
             -> HPL_pdpanel_free
             -> HPL_pdpanel_init
             -> HPL_pdfact
                -> HPL_pdrpanllT
                   -> HPL_pdpanllT
                      -> HPL_dlocmax
                      -> HPL_pdmxswp
                      -> HPL_dlocswpT
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
           -> HPL_pdtrsv
     -> HPL_pdlange / HPL_all_reduce / HPL_broadcast / HPL_reduce
```

## 6. 本条路径的通信总结

| 阶段 | 主要函数 | 语义 | 实现 | 算子 |
| --- | --- | --- | --- | --- |
| 内存失败检查 | `HPL_all_reduce` | 全归约 | HPL 集合封装 | `HPL_max` |
| 计时同步 | `HPL_barrier` | 屏障 | HPL 集合封装 | 无 |
| panel 分解中的主元竞争 | `HPL_pdmxswp` | 融合归约/传播语义 | 以点对点为主 | 主元最大值比较 |
| panel 广播 | `HPL_bcast -> HPL_bcast_1ring` | 广播 | 点对点环形转发 | 无 |
| 更新阶段的 U 重分发 | `HPL_pdlaswp00T` | 行交换 + U 传播 | 点对点 | 无 |
| 回代主流水 | `HPL_send/HPL_recv` | 点对点 | 点对点 | 无 |
| 残差列范数 | `HPL_all_reduce` | 全归约 | HPL 集合封装 | `HPL_max` |
| 残差复制 | `HPL_broadcast` | 广播 | HPL 集合封装 | 无 |
| 残差累加 | `HPL_reduce` | 归约 | HPL 集合封装 | `HPL_sum` |

## 7. 本文最值得记住的三个结论

### 结论 1：配置选项不等于真实执行路径

本次虽然配置上选择了：

- `RFACT = left`

但由于：

- `NB = 1`
- `NBMIN = 2`

真实执行时递归立刻终止，直接落到：

- `HPL_pdpanllT`

### 结论 2：`DEPTH = 0` 让主线最清楚

本次配置走的是：

- `HPL_pdgesv0`

所以主循环严格是：

```text
panel init -> panel factorization -> panel broadcast -> trailing update
```

非常适合做代码梳理与考试回答。

### 结论 3：HPL 的“集合语义”很多时候不是标准 MPI collective

例如：

- panel 广播在语义上是 broadcast
- 但本次实际实现是 `HPL_bcast_1ring`
- 本质是点对点环形传播

这正是 HPL 的一个核心设计思想：

> 不把高性能通信模式完全交给 MPI 默认 collective，而是自己把广播/归约语义映射到可调拓扑与可控消息调度上。

