# HPL_pdtest 矩阵创建与分布过程详解

本文专门解释 `testing/ptest/HPL_pdtest.c` 中矩阵创建、内存分配、块循环分布和随机填充的过程，重点覆盖 `HPL_pdtest.c` 第 237-308 行附近的代码，以及其中用到的 `HPL_numroc`、`HPL_pdmatgen` 等接口。

## 1. 总体背景

HPL 求解的是线性系统：

```text
A x = b
```

在测试程序里，系数矩阵 `A` 和右端项 `b` 被合并成一个增广矩阵：

```text
[ A | b ]
```

如果问题规模是 `N`，那么全局逻辑矩阵大小是：

```text
N x (N+1)
```

其中前 `N` 列是系数矩阵 `A`，最后一列是右端项 `b`。HPL 不会在某一个进程上创建完整的 `N x (N+1)` 矩阵再分发；它把矩阵按 `NB x NB` 块切分，并以二维块循环方式分布到 `P x Q` 进程网格上。每个进程只分配和生成自己负责的本地矩阵片段。

`www/algorithm.html` 中也描述了这个基本思想：数据分布到二维 `P x Q` 进程网格上，矩阵先被逻辑划分成 `nb x nb` 块，然后在两个维度上循环分配。

## 2. 进程网格信息

相关代码：

```c
(void) HPL_grid_info( GRID, &nprow, &npcol, &myrow, &mycol );
```

`HPL_grid_info` 从 `GRID` 中取出当前二维进程网格信息：

| 变量 | 含义 |
| --- | --- |
| `nprow` | 进程网格的行数，也就是 `P` |
| `npcol` | 进程网格的列数，也就是 `Q` |
| `myrow` | 当前进程在进程网格中的行坐标 |
| `mycol` | 当前进程在进程网格中的列坐标 |

二维进程网格是 HPL 数据分布的基础。矩阵的全局行按块循环映射到进程行，矩阵的全局列按块循环映射到进程列。因此，一个进程 `(myrow, mycol)` 拥有的是某些全局行块和某些全局列块交叉形成的本地子矩阵。

## 3. HPL_T_pmat 中的关键字段

`HPL_T_pmat` 定义在 `include/hpl_pgesv.h` 中，和本段代码直接相关的字段如下：

| 字段 | 含义 |
| --- | --- |
| `mat.A` | 指向本进程本地矩阵 `[A | b]` 片段的指针 |
| `mat.X` | 指向本进程本地解向量片段的指针 |
| `mat.n` | 全局问题规模 `N` |
| `mat.nb` | 分块大小 `NB` |
| `mat.ld` | 本地矩阵的 leading dimension |
| `mat.mp` | 当前进程拥有的本地行数 |
| `mat.nq` | 当前进程本地矩阵缓冲区的列数，等于本地 `A` 列数加 1 |
| `mat.info` | 求解过程中的状态标志 |

初始化代码：

```c
mat.n  = N;
mat.nb = NB;
mat.info = 0;
```

这一步只保存全局问题规模、块大小，并把求解状态置为正常。

## 4. HPL_numroc 的作用

相关代码：

```c
mat.mp = HPL_numroc( N, NB, NB, myrow, 0, nprow );
nq     = HPL_numroc( N, NB, NB, mycol, 0, npcol );
mat.nq = nq + 1;
```

`HPL_numroc` 的作用是：在块循环分布规则下，计算某个进程坐标在某一个维度上能拿到多少个全局元素。

接口原型：

```c
int HPL_numroc(
   const int N,
   const int INB,
   const int NB,
   const int PROC,
   const int SRCPROC,
   const int NPROCS
);
```

参数含义：

| 参数 | 含义 |
| --- | --- |
| `N` | 这一维度上的全局元素个数，可以是行数，也可以是列数 |
| `INB` | 第一个块的大小 |
| `NB` | 后续规则块的大小 |
| `PROC` | 要查询的进程坐标 |
| `SRCPROC` | 第一个块所在的源进程坐标 |
| `NPROCS` | 这一维度上参与分布的进程数量 |

在 `HPL_pdtest` 中，`INB` 和 `NB` 都传入 `NB`，表示第一个块和后续块大小相同。`SRCPROC` 传入 `0`，表示全局第一个行块和第一个列块都从坐标 0 的进程行/列开始分配。

### 4.1 行维度计算

```c
mat.mp = HPL_numroc( N, NB, NB, myrow, 0, nprow );
```

这表示：

```text
把 N 个全局行，以 NB 行为一块，从进程行 0 开始，循环分给 nprow 个进程行。
当前进程所在的 myrow 这一行，一共能获得多少个全局行？
```

返回值写入 `mat.mp`。因此 `mat.mp` 是当前进程的本地行数。

### 4.2 列维度计算

```c
nq = HPL_numroc( N, NB, NB, mycol, 0, npcol );
```

这表示：

```text
把 N 个全局 A 列，以 NB 列为一块，从进程列 0 开始，循环分给 npcol 个进程列。
当前进程所在的 mycol 这一列，一共能获得多少个全局 A 列？
```

返回值写入临时变量 `nq`。注意这里传入的是 `N`，不是 `N+1`，因为 `nq` 表示当前进程拥有的系数矩阵 `A` 的本地列数。

随后：

```c
mat.nq = nq + 1;
```

`mat.nq` 比 `nq` 多 1，是为了在本地矩阵缓冲区中额外容纳右端项 `b`。在当前实现中，每个进程列都多预留一列本地存储，便于后续求解和校验逻辑用统一的 `[A | b]` 布局处理。

### 4.3 HPL_numroc 与 HPL_numrocI

`HPL_numroc` 本身只是薄封装：

```c
return( HPL_numrocI( N, 0, INB, NB, PROC, SRCPROC, NPROCS ) );
```

真正的计算在 `src/pauxil/HPL_numrocI.c`。`HPL_numrocI` 支持从任意全局下标 `I` 开始计算局部元素个数，而 `HPL_numroc` 固定从全局下标 0 开始。

`HPL_numrocI` 的核心逻辑是：

1. 如果 `SRCPROC == -1` 或 `NPROCS == 1`，说明这一维不分布，直接返回 `N`。
2. 找到全局起点 `I` 属于哪个源进程块。
3. 计算总共有多少个完整块。
4. 计算当前 `PROC` 相对源进程的循环距离。
5. 根据循环距离和尾块大小，返回当前进程拥有的本地元素数。

换句话说，`HPL_numroc` 不移动数据，它只回答“按照块循环规则，这个进程应该拥有多少行/列”。

## 5. 本地 leading dimension: mat.ld

相关代码：

```c
mat.ld = ( ( Mmax( 1, mat.mp ) - 1 ) / ALGO->align ) * ALGO->align;
do
{
   ii = ( mat.ld += ALGO->align ); ip2 = 1;
   while( ii > 1 ) { ii >>= 1; ip2 <<= 1; }
}
while( mat.ld == ip2 );
```

`mat.ld` 是本地矩阵的 leading dimension。HPL 本地矩阵按列主序存储，所以 `mat.ld` 是相邻两列起始地址之间的元素跨度。

它必须满足：

```text
mat.ld >= max(1, mat.mp)
```

也就是说，本地每一列至少要能存下 `mat.mp` 个行元素。

这段代码还额外保证：

1. `mat.ld` 是 `ALGO->align` 的整数倍。
2. `mat.ld` 不是 2 的幂。

其中 `ALGO->align` 来自 `HPL.dat` 的 alignment 配置，单位是 double precision words。避免 `mat.ld` 是 2 的幂，是为了降低某些机器上缓存或内存 bank 周期性冲突的概率。

变量作用：

| 变量 | 含义 |
| --- | --- |
| `ii` | 临时保存当前候选 `mat.ld`，用于判断是否为 2 的幂 |
| `ip2` | 通过不断左移构造出的不超过 `ii` 的最大 2 的幂 |
| `ALGO->align` | 本地缓冲区对齐粒度 |

## 6. 本地内存分配

相关代码：

```c
vptr = (void*)malloc( ( (size_t)(ALGO->align) +
                        (size_t)(mat.ld+1) * (size_t)(mat.nq) ) *
                      sizeof(double) );
```

这次 `malloc` 一次性分配本进程需要的矩阵、右端项和解向量空间。

内存量可以拆成：

```text
ALGO->align + (mat.ld + 1) * mat.nq
= ALGO->align + mat.ld * mat.nq + mat.nq
```

含义如下：

| 部分 | 用途 |
| --- | --- |
| `ALGO->align` | 给 `HPL_PTR` 做地址对齐时预留的冗余空间 |
| `mat.ld * mat.nq` | 存放本地 `[A | b]` |
| `mat.nq` | 存放本地解向量片段 `mat.X` |

随后用 `HPL_all_reduce` 检查所有进程是否都分配成功：

```c
info[0] = (vptr == NULL);
info[1] = myrow;
info[2] = mycol;
(void) HPL_all_reduce( (void *)(info), 3, HPL_INT, HPL_max,
                       GRID->all_comm );
```

变量作用：

| 变量 | 含义 |
| --- | --- |
| `vptr` | 原始 malloc 指针 |
| `info[0]` | 当前进程是否分配失败，失败为 1，成功为 0 |
| `info[1]` | 当前进程行坐标，用于错误报告 |
| `info[2]` | 当前进程列坐标，用于错误报告 |
| `GRID->all_comm` | 覆盖整个 `P x Q` 进程网格的通信器 |

这里使用 `HPL_max` 做归约。只要任意进程的 `info[0]` 为 1，归约后的 `info[0]` 就会是 1，所有进程都会知道需要跳过本次测试。

## 7. mat.A、mat.X 与 Mptr

相关代码：

```c
mat.A  = (double *)HPL_PTR( vptr,
                            ((size_t)(ALGO->align) * sizeof(double) ) );
mat.X  = Mptr( mat.A, 0, mat.nq, mat.ld );
```

`HPL_PTR` 定义在 `include/hpl_misc.h` 中，用于把原始指针对齐到指定字节边界。

`Mptr` 也定义在 `include/hpl_misc.h` 中：

```c
#define Mptr( a_, i_, j_, lda_ ) \
   ( (a_) + (size_t)(i_) + (size_t)(j_)*(size_t)(lda_) )
```

它表示列主序矩阵中本地元素 `(i, j)` 的地址。

因此：

```c
Mptr( mat.A, 0, mat.nq, mat.ld )
```

表示从 `mat.A` 出发，跳到第 `mat.nq` 列的第 0 行。由于本地 `[A | b]` 占据了第 `0` 到 `mat.nq - 1` 列，所以第 `mat.nq` 列正好位于矩阵缓冲区之后，可作为 `mat.X` 的存储起点。

内存布局可以理解为：

```text
vptr 原始地址
  |
  |  对齐冗余
  v
mat.A -> 本地 [A | b]，尺寸约为 mat.ld x mat.nq
  |
  v
mat.X -> 本地解向量片段，长度约为 mat.nq
```

## 8. HPL_pdmatgen 的作用

相关代码：

```c
HPL_pdmatgen( GRID, N, N+1, NB, mat.A, mat.ld, HPL_ISEED );
```

接口原型：

```c
void HPL_pdmatgen(
   const HPL_T_grid *GRID,
   const int M,
   const int N,
   const int NB,
   double *A,
   const int LDA,
   const int ISEED
);
```

参数含义：

| 参数 | 含义 |
| --- | --- |
| `GRID` | 当前二维进程网格 |
| `M` | 全局矩阵行数，这里是 `N` |
| `N` | 全局矩阵列数，这里传入 `N+1`，表示 `[A | b]` |
| `NB` | 块循环分布的块大小 |
| `A` | 当前进程本地矩阵缓冲区，也就是 `mat.A` |
| `LDA` | 本地 leading dimension，也就是 `mat.ld` |
| `ISEED` | 伪随机数种子，HPL 测试中为 `HPL_ISEED` |

`HPL_pdmatgen` 生成的是“并行随机矩阵”。它不是由某个进程生成完整矩阵再用 MPI 分发，而是所有进程使用同一个随机数生成规则、同一个种子、同一个块循环分布规则，各自跳到自己负责的全局位置，直接生成本地元素。

这样做有两个好处：

1. 生成阶段不需要集中式大矩阵，也不需要大量分发通信。
2. 所有进程本地片段拼起来，仍然等价于一个确定的全局随机矩阵。

## 9. HPL_pdmatgen 内部如何定位本地数据

在 `testing/pmatgen/HPL_pdmatgen.c` 中，函数首先再次获取进程网格：

```c
(void) HPL_grid_info( GRID, &nprow, &npcol, &myrow, &mycol );
```

然后计算当前进程在这个全局 `M x N` 矩阵中拥有多少本地行和本地列：

```c
Mnumroc( mp, M, NB, NB, myrow, 0, nprow );
Mnumroc( nq, N, NB, NB, mycol, 0, npcol );
```

这里的 `Mnumroc` 是宏形式的本地元素数计算，语义和 `HPL_numroc` 一致。

接着计算本地块数量和最后一个块大小：

```c
mblks = ( mp + NB - 1 ) / NB;
lmb = mp - ( ( mp - 1 ) / NB ) * NB;
nblks = ( nq + NB - 1 ) / NB;
lnb = nq - ( ( nq - 1 ) / NB ) * NB;
```

变量作用：

| 变量 | 含义 |
| --- | --- |
| `mp` | 当前进程本地行数 |
| `nq` | 当前进程本地列数 |
| `mblks` | 本地行方向块数 |
| `nblks` | 本地列方向块数 |
| `lmb` | 本地最后一个行块的实际大小 |
| `lnb` | 本地最后一个列块的实际大小 |

如果全局大小不能被 `NB` 整除，最后一个块就可能小于 `NB`。

## 10. HPL_pdmatgen 如何避免通信

`HPL_pdmatgen` 使用线性同余伪随机数生成器：

```text
X(n+1) = (a * X(n) + c) mod m
```

为了让每个进程能直接生成自己的本地元素，它需要在随机序列中进行“跳跃”。相关变量包括：

```c
jump1 = 1;
jump2 = nprow * NB;
jump3 = M;
jump4 = npcol * NB;
jump5 = NB;
jump6 = mycol;
jump7 = myrow * NB;
```

这些 jump 的作用是根据二维块循环布局，跳过那些属于其他进程行或进程列的全局元素。

随后调用多次：

```c
HPL_xjumpm(...)
```

`HPL_xjumpm` 会计算“跳过若干个随机数之后”的等效乘子和加数。这样后续循环中就可以用 `HPL_jumpit` 快速跳到下一个本进程应生成的位置。

最终填充循环是：

```c
for( jblk = 0; jblk < nblks; jblk++ )
{
   jb = ( jblk == nblks - 1 ? lnb : NB );
   for( jk = 0; jk < jb; jk++ )
   {
      for( iblk = 0; iblk < mblks; iblk++ )
      {
         ib = ( iblk == mblks - 1 ? lmb : NB );
         for( ik = 0; ik < ib; A++, ik++ ) *A = HPL_rand();
         HPL_jumpit( ia2, ic2, ib1, iran2 );
         ib1[0] = iran2[0]; ib1[1] = iran2[1];
      }
      A += LDA - mp;
      HPL_jumpit( ia3, ic3, ib2, iran3 );
      ib1[0] = iran3[0]; ib1[1] = iran3[1];
      ib2[0] = iran3[0]; ib2[1] = iran3[1];
   }
   HPL_jumpit( ia4, ic4, ib3, iran4 );
   ib1[0] = iran4[0]; ib1[1] = iran4[1];
   ib2[0] = iran4[0]; ib2[1] = iran4[1];
   ib3[0] = iran4[0]; ib3[1] = iran4[1];
}
```

循环层次可以这样理解：

| 循环变量 | 含义 |
| --- | --- |
| `jblk` | 本地列块编号 |
| `jk` | 当前本地列块内部的列偏移 |
| `iblk` | 本地行块编号 |
| `ik` | 当前本地行块内部的行偏移 |

因为本地矩阵是列主序，所以最内层 `A++` 连续写入当前本地列中的一段行元素。每写完一个本地行块，就用 `HPL_jumpit` 跳过全局随机序列中属于其他进程行的元素；每写完一整列，就跳到下一列对应的随机序列位置；每写完一个本地列块，就跳到下一个属于当前进程列的全局列块。

这就是 `HPL_pdmatgen` 能够“本地独立生成、全局保持一致”的关键。

## 11. 本段代码中各变量汇总

| 变量 | 所在函数 | 作用 |
| --- | --- | --- |
| `N` | `HPL_pdtest` | 全局问题规模，系数矩阵 `A` 是 `N x N` |
| `NB` | `HPL_pdtest` | 分布块大小，也是 LU 面板宽度 |
| `GRID` | `HPL_pdtest` / `HPL_pdmatgen` | 二维进程网格描述符 |
| `ALGO` | `HPL_pdtest` | 算法参数，包括 `align` 等 |
| `nprow` | `HPL_pdtest` / `HPL_pdmatgen` | 进程网格行数 |
| `npcol` | `HPL_pdtest` / `HPL_pdmatgen` | 进程网格列数 |
| `myrow` | `HPL_pdtest` / `HPL_pdmatgen` | 当前进程行坐标 |
| `mycol` | `HPL_pdtest` / `HPL_pdmatgen` | 当前进程列坐标 |
| `mat.n` | `HPL_pdtest` | 保存全局问题规模 |
| `mat.nb` | `HPL_pdtest` | 保存分块大小 |
| `mat.info` | `HPL_pdtest` | 求解状态标志 |
| `mat.mp` | `HPL_pdtest` | 当前进程本地行数 |
| `nq` | `HPL_pdtest` | 当前进程拥有的本地 `A` 列数 |
| `mat.nq` | `HPL_pdtest` | 本地 `[A | b]` 缓冲区列数，等于 `nq + 1` |
| `mat.ld` | `HPL_pdtest` | 本地列主序矩阵的列跨度 |
| `ii` | `HPL_pdtest` | 判断 `mat.ld` 是否为 2 的幂时使用的临时值 |
| `ip2` | `HPL_pdtest` | 不超过 `ii` 的最大 2 的幂 |
| `vptr` | `HPL_pdtest` | 原始动态内存指针 |
| `info` | `HPL_pdtest` | 全局内存分配失败检测缓冲区 |
| `mat.A` | `HPL_pdtest` | 对齐后的本地 `[A | b]` 起始地址 |
| `mat.X` | `HPL_pdtest` | 本地解向量片段起始地址 |
| `HPL_ISEED` | `HPL_pdtest` | 固定随机种子，保证测试可复现 |
| `mp` | `HPL_pdmatgen` | 当前进程对输入矩阵 `M` 维度的本地行数 |
| `nq` | `HPL_pdmatgen` | 当前进程对输入矩阵 `N` 维度的本地列数 |
| `mblks` / `nblks` | `HPL_pdmatgen` | 本地行/列方向块数 |
| `lmb` / `lnb` | `HPL_pdmatgen` | 本地最后一个行/列块大小 |
| `jump1` 到 `jump7` | `HPL_pdmatgen` | 伪随机序列跳跃距离，用于定位本地负责的全局元素 |
| `jblk`, `jk`, `iblk`, `ik` | `HPL_pdmatgen` | 本地块与块内元素遍历变量 |

## 12. 一句话总结

`HPL_pdtest` 通过 `HPL_numroc` 计算每个进程应该拥有的本地矩阵尺寸，只分配这部分本地存储；随后 `HPL_pdmatgen` 利用相同的二维块循环规则和可跳跃的伪随机序列，在每个进程上直接生成本地 `[A | b]` 片段。整个过程避免了集中式完整矩阵和生成阶段的大规模分发通信，同时保证所有本地片段组合起来就是同一个确定的全局测试矩阵。
