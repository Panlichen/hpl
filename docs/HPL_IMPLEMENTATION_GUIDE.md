# HPL In This Project: Implementation Walkthrough
# 本项目中的 HPL：实现过程详解

## 1. Goal and scope
## 1. 目标与范围

This document explains how the project implements HPL from:
本文说明本项目是如何实现 HPL 的，内容覆盖以下方面：

- program entry and test driver
- 程序入口与测试驱动
- process-grid construction and data distribution
- 进程网格构建与数据分布
- LU factorization main loop
- LU 分解主循环
- panel factorization
- 面板分解
- panel broadcast
- 面板广播
- trailing matrix update
- 尾随矩阵更新
- upper-triangular solve
- 上三角求解
- result verification
- 结果校验

The focus is on communication:
重点关注通信部分：

- where communication happens
- 通信发生在什么地方
- whether it is point-to-point or collective in semantics
- 它在语义上属于点对点通信还是集合通信
- which operator is used when a reduction is involved
- 当涉及归约时使用了什么算子
- why that communication pattern is chosen
- 为什么选择这种通信方式

## 2. High-level call chain
## 2. 高层调用链

The execution path for one HPL run is:
一次 HPL 运行的执行路径如下：

1. `testing/ptest/HPL_pddriver.c`
1. `testing/ptest/HPL_pddriver.c`

   Reads `HPL.dat`, enumerates algorithm choices, builds `HPL_T_palg`, creates a process grid, and calls `HPL_pdtest`.
   读取 `HPL.dat`，枚举算法参数选项，构造 `HPL_T_palg`，创建进程网格，并调用 `HPL_pdtest`。

2. `testing/ptest/HPL_pdtest.c`
2. `testing/ptest/HPL_pdtest.c`

   Allocates the distributed matrix `[A | b]`, generates test data, times `HPL_pdgesv`, then verifies the result.
   分配分布式矩阵 `[A | b]`，生成测试数据，计时 `HPL_pdgesv`，随后校验结果。

3. `src/pgesv/HPL_pdgesv.c`
3. `src/pgesv/HPL_pdgesv.c`

   Chooses the LU schedule:
   选择 LU 分解调度方式：

   - `HPL_pdgesv0` for no look-ahead
   - `HPL_pdgesv0`：无 look-ahead
   - `HPL_pdgesvK2` for look-ahead
   - `HPL_pdgesvK2`：带 look-ahead

4. `src/pfact/*`
4. `src/pfact/*`

   Factorizes each panel, finds pivots, swaps rows, and prepares panel payloads.
   对每个面板做分解、寻找主元、交换行，并准备面板通信负载。

5. `src/comm/*`
5. `src/comm/*`

   Broadcasts the current panel along the process row with a selectable topology.
   按可选拓扑在进程行上广播当前面板。

6. `src/pgesv/HPL_pdupdate*.c`
6. `src/pgesv/HPL_pdupdate*.c`

   Applies row interchanges, forms the row panel `U`, and updates the trailing matrix.
   应用行交换，构造行面板 `U`，并更新尾随矩阵。

7. `src/pgesv/HPL_pdtrsv.c`
7. `src/pgesv/HPL_pdtrsv.c`

   Solves the upper-triangular system after the LU factorization is complete.
   在 LU 分解完成后求解上三角线性系统。

## 3. Key data structures
## 3. 关键数据结构

### 3.1 Process grid: `HPL_T_grid`
### 3.1 进程网格：`HPL_T_grid`

Defined in `include/hpl_grid.h`.
定义于 `include/hpl_grid.h`。

Important fields:
重要字段如下：

- `all_comm`: communicator for the whole `P x Q` grid
- `all_comm`：整个 `P x Q` 网格的通信器
- `row_comm`: communicator for one process row
- `row_comm`：单个进程行的通信器
- `col_comm`: communicator for one process column
- `col_comm`：单个进程列的通信器
- `myrow`, `mycol`: local grid coordinates
- `myrow`, `mycol`：本地进程在网格中的坐标
- `row_ip2`, `col_ip2`, `row_hdim`, `col_hdim`: precomputed helpers for custom tree / hypercube exchanges
- `row_ip2`, `col_ip2`, `row_hdim`, `col_hdim`：为自定义树形或超立方交换预计算的辅助参数

### 3.2 Panel descriptor: `HPL_T_panel`
### 3.2 面板描述符：`HPL_T_panel`

Defined in `include/hpl_panel.h`.
定义于 `include/hpl_panel.h`。

Important fields:
重要字段如下：

- `A`: local pointer to the trailing matrix block starting at the current panel
- `A`：指向从当前面板开始的本地尾随矩阵块
- `L1`: replicated `jb x jb` diagonal block
- `L1`：复制的 `jb x jb` 对角块
- `L2`: subdiagonal block used during trailing update
- `L2`：尾随更新时使用的次对角块
- `DPIV`: replicated pivot indices
- `DPIV`：复制的主元索引
- `DINFO`: replicated info flag
- `DINFO`：复制的状态标志
- `U`: workspace used for redistributed row panel data
- `U`：用于重分发行面板数据的工作区
- `msgid`: message id for row broadcast
- `msgid`：行广播使用的消息编号

The panel data structure is not just metadata.
面板数据结构不仅仅是元数据。

It is also the communication container for the panel broadcast.
它同时也是面板广播的通信载体。

## 4. How the distributed matrix is laid out
## 4. 分布式矩阵是如何布局的

HPL uses 2D block-cyclic distribution.
HPL 使用二维块循环分布。

- Global matrix `A` is partitioned into blocks of size `NB x NB`
- 全局矩阵 `A` 被划分为大小为 `NB x NB` 的块
- Blocks are assigned cyclically over the `P x Q` process grid
- 这些块以循环方式分配到 `P x Q` 进程网格上
- Each rank owns a scattered subset of matrix blocks
- 每个 rank 持有若干离散分布的矩阵块子集

Why this distribution is used:
采用这种分布方式的原因是：

- balances work across ranks
- 在各个 rank 之间平衡工作量
- balances memory footprint
- 平衡内存占用
- keeps Level-3 BLAS opportunities high
- 尽可能保持 Level-3 BLAS 的使用机会
- prevents one process from becoming the permanent owner of all late panels
- 避免某个进程永久拥有后续所有面板

The helper routines that compute ownership and local/global index mapping include:
用于计算块归属关系以及局部/全局索引映射的辅助例程包括：

- `HPL_numroc`, `HPL_numrocI`
- `HPL_numroc`, `HPL_numrocI`
- `HPL_infog2l`
- `HPL_infog2l`
- `HPL_indxg2p`, `HPL_indxg2l`, `HPL_indxl2g`
- `HPL_indxg2p`, `HPL_indxg2l`, `HPL_indxl2g`

## 5. Main LU schedule
## 5. LU 主调度流程

### 5.1 No look-ahead: `HPL_pdgesv0`
### 5.1 无 look-ahead：`HPL_pdgesv0`

For each panel at global column `j`:
对于全局列 `j` 处的每个面板：

1. initialize panel descriptor
1. 初始化面板描述符
2. factor the panel in the owner process column
2. 在拥有该面板的进程列中完成面板分解
3. broadcast the panel across the owner process row
3. 沿拥有该面板的进程行广播该面板
4. update the trailing submatrix
4. 更新尾随子矩阵

This path is conceptually simpler and is the best starting point to understand HPL.
这条路径在概念上更简单，是理解 HPL 的最佳起点。

### 5.2 Look-ahead: `HPL_pdgesvK2`
### 5.2 带 Look-ahead：`HPL_pdgesvK2`

This path overlaps:
这条路径会重叠执行以下工作：

- factorization of the current panel
- 当前面板的分解
- broadcast of the next panel
- 下一面板的广播
- remaining updates from previously factored panels
- 先前已分解面板的剩余更新

The `panel[0..depth]` array acts as a software pipeline of descriptors.
`panel[0..depth]` 数组充当一个软件流水线中的描述符队列。

Why look-ahead is used:
使用 look-ahead 的原因是：

- panel factorization is latency-sensitive
- 面板分解对时延非常敏感
- trailing update is compute-heavy and can hide communication
- 尾随更新计算量大，可以隐藏通信开销
- overlap improves utilization of both network and BLAS kernels
- 重叠执行可以同时提高网络和 BLAS 内核的利用率

## 6. Communication map by phase
## 6. 按阶段划分的通信图谱

### 6.1 Communicator usage
### 6.1 通信器使用方式

- `all_comm`
- `all_comm`

  Used for run-level collectives such as timing aggregation, parameter distribution, global checks.
  用于运行级别的集合操作，例如计时聚合、参数分发和全局检查。

- `row_comm`
- `row_comm`

  Used mainly for row-wise panel broadcasts and row-wise reductions.
  主要用于按进程行进行的面板广播和按行归约。

- `col_comm`
- `col_comm`

  Used mainly for pivot search, fused swap/broadcast inside panel factorization, `U` redistribution, and triangular solve pipeline.
  主要用于主元搜索、面板分解内部融合的交换/广播、`U` 的重分发以及上三角求解流水。

### 6.2 Summary table
### 6.2 汇总表

| Phase | Main function(s) | Communicator | Semantics | MPI style |
| 阶段 | 主要函数 | 通信器 | 语义 | MPI 风格 |
| --- | --- | --- | --- | --- |
| Grid creation | `HPL_grid_init` | world -> grid splits | communicator creation | `MPI_Comm_split` |
| 网格创建 | `HPL_grid_init` | world -> grid splits | 通信器创建 | `MPI_Comm_split` |
| Input / setup | `HPL_pdinfo` | `MPI_COMM_WORLD` | broadcast / all-reduce | HPL custom collective on top of p2p |
| 输入 / 初始化 | `HPL_pdinfo` | `MPI_COMM_WORLD` | 广播 / 全归约 | HPL 基于点对点实现的自定义集合操作 |
| Panel pivot search | `HPL_dlocmax` + `HPL_pdmxswp` | `col_comm` | fused max-selection + pivot-row movement | point-to-point |
| 面板主元搜索 | `HPL_dlocmax` + `HPL_pdmxswp` | `col_comm` | 融合的最大值选择 + 主元行移动 | 点对点 |
| Panel broadcast | `HPL_binit`, `HPL_bcast`, `HPL_bwait` | row direction over grid row | broadcast | point-to-point topology |
| 面板广播 | `HPL_binit`, `HPL_bcast`, `HPL_bwait` | 沿网格进程行方向 | 广播 | 基于点对点的拓扑路由 |
| Row swap + U distribution | `HPL_pdlaswp00*`, `HPL_pdlaswp01*`, `HPL_spread*`, `HPL_roll*` | `col_comm` | custom redistribution | point-to-point |
| 行交换 + U 分发 | `HPL_pdlaswp00*`, `HPL_pdlaswp01*`, `HPL_spread*`, `HPL_roll*` | `col_comm` | 自定义重分发 | 点对点 |
| Trailing update polling | `HPL_pdupdate*` | both row and column paths | overlap/progress | mixed, mostly p2p |
| 尾随更新期间的轮询 | `HPL_pdupdate*` | 同时涉及行与列路径 | 重叠 / 推进 | 混合型，主要是点对点 |
| Triangular solve | `HPL_pdtrsv` | `row_comm`, `col_comm` | pipeline + final broadcast | mixed |
| 上三角求解 | `HPL_pdtrsv` | `row_comm`, `col_comm` | 流水线 + 最终广播 | 混合型 |
| Timing / norms / residual | `HPL_all_reduce`, `HPL_reduce`, `HPL_broadcast` | `all_comm`, `row_comm`, `col_comm` | max/sum/min/broadcast | collective semantics, custom p2p |
| 计时 / 范数 / 残差 | `HPL_all_reduce`, `HPL_reduce`, `HPL_broadcast` | `all_comm`, `row_comm`, `col_comm` | max/sum/min/广播 | 集合语义，自定义点对点实现 |

## 7. Detailed communication analysis
## 7. 详细通信分析

### 7.1 Process-grid construction
### 7.1 进程网格构建

File: `src/grid/HPL_grid_init.c`
文件：`src/grid/HPL_grid_init.c`

Communication:
通信包括：

- `MPI_Comm_split` to create `all_comm`
- 使用 `MPI_Comm_split` 创建 `all_comm`
- `MPI_Comm_split` to create `row_comm`
- 使用 `MPI_Comm_split` 创建 `row_comm`
- `MPI_Comm_split` to create `col_comm`
- 使用 `MPI_Comm_split` 创建 `col_comm`

Type:
类型：

- true MPI collective on communicator construction
- 在通信器构建阶段使用真正的 MPI 集合操作

Why:
原因：

- HPL wants three layers of communication domains
- HPL 希望建立三层通信域
- algorithmic phases can then talk only to the ranks that matter
- 这样各个算法阶段就只需要和真正相关的 rank 通信

### 7.2 Panel factorization
### 7.2 面板分解

Main files:
主要文件：

- `src/pfact/HPL_pdfact.c`
- `src/pfact/HPL_pdfact.c`
- `src/pfact/HPL_dlocmax.c`
- `src/pfact/HPL_dlocmax.c`
- `src/pfact/HPL_pdmxswp.c`
- `src/pfact/HPL_pdmxswp.c`

#### Local pivot search
#### 局部主元搜索

`HPL_dlocmax` searches the current local column segment for the maximum absolute value.
`HPL_dlocmax` 在当前本地列段中搜索绝对值最大的元素。

Local result includes:
本地结果包括：

- value
- 数值
- local row index
- 局部行索引
- global row index
- 全局行索引
- owner process row
- 所属进程行

No communication happens here.
这里不发生通信。

#### Global pivot selection and pivot-row movement
#### 全局主元选择与主元行移动

`HPL_pdmxswp` performs the important communication.
`HPL_pdmxswp` 执行这一关键通信过程。

Semantics:
语义上，它完成：

- find the globally best pivot row in the current process column
- 在当前进程列中找到全局最优的主元行
- move the pivot row so every process row in that process column can use it
- 移动主元行，使该进程列中的每个进程行都能使用这条主元行

Actual communication style:
实际通信方式：

- point-to-point only
- 完全是点对点通信
- built from `HPL_send`, `HPL_recv`, `HPL_sdrv`
- 基于 `HPL_send`、`HPL_recv`、`HPL_sdrv` 构建
- these wrap `MPI_Send`, `MPI_Recv`, and paired send/recv logic
- 它们封装了 `MPI_Send`、`MPI_Recv` 以及成对的收发逻辑

Why HPL does not use `MPI_Reduce + MPI_Bcast` here:
为什么这里不直接使用 `MPI_Reduce + MPI_Bcast`：

- the metadata to reduce is tiny, but the pivot row payload is not
- 需要归约的元数据很小，但主元行本身的负载并不小
- the chosen pivot row itself must move together with the winning metadata
- 被选中的主元行必须和获胜的元数据一起移动
- a fused custom exchange reduces synchronization and message startup cost
- 融合式的自定义交换可以减少同步次数和消息启动开销

Pattern:
模式上，它表现为：

- non-power-of-two cleanup stage if needed
- 如果需要，先处理非 2 的幂规模部分
- hypercube / binary-exchange style reduction among process rows
- 在进程行之间进行超立方或二进制交换风格的归约
- final fan-out to remaining processes if `nprow` is not a power of two
- 如果 `nprow` 不是 2 的幂，则最后再把结果扩散给剩余进程

Operator:
算子方面：

- logical operator is max by absolute value
- 逻辑算子是按绝对值取最大
- tie-break prefers smaller owner row coordinate
- 若并列，则优先选择进程行坐标更小的候选
- this is not expressed as `HPL_max`; it is embedded in custom compare logic inside `HPL_pdmxswp`
- 这一逻辑并没有直接写成 `HPL_max`，而是嵌入在 `HPL_pdmxswp` 的自定义比较逻辑中

### 7.3 Panel broadcast along process rows
### 7.3 沿进程行的面板广播

Main files:
主要文件：

- `src/comm/HPL_binit.c`
- `src/comm/HPL_binit.c`
- `src/comm/HPL_bcast.c`
- `src/comm/HPL_bcast.c`
- `src/comm/HPL_bwait.c`
- `src/comm/HPL_bwait.c`
- `src/comm/HPL_1ring.c`
- `src/comm/HPL_1ring.c`
- `src/comm/HPL_1rinM.c`
- `src/comm/HPL_1rinM.c`
- `src/comm/HPL_2ring.c`
- `src/comm/HPL_2ring.c`
- `src/comm/HPL_2rinM.c`
- `src/comm/HPL_2rinM.c`
- `src/comm/HPL_blong.c`
- `src/comm/HPL_blong.c`
- `src/comm/HPL_blonM.c`
- `src/comm/HPL_blonM.c`

Semantics:
语义：

- broadcast the current panel from owner process column across the process row
- 将当前面板从拥有它的进程列广播到整条进程行

Actual implementation:
实际实现：

- custom point-to-point topologies
- 自定义的点对点拓扑
- no direct `MPI_Bcast`
- 不直接调用 `MPI_Bcast`

Supported virtual topologies:
支持的虚拟拓扑包括：

- `1ring`
- `1ring`
- `1ring modified`
- `1ring modified`
- `2ring`
- `2ring`
- `2ring modified`
- `2ring modified`
- `long`
- `long`
- `long modified`
- `long modified`

Why custom topologies are used:
为什么要使用自定义拓扑：

- panel payloads are structured and latency-sensitive
- 面板负载有结构性，而且对时延很敏感
- different machines favor different message routes
- 不同机器更适合不同的消息路由
- HPL tuning exposes topology choice in `HPL.dat`
- HPL 调优把拓扑选择暴露为 `HPL.dat` 中的一个参数

Why `binit/bcast/bwait` are split:
为什么要把 `binit/bcast/bwait` 拆开：

- they let HPL poll the progress of panel forwarding while useful compute continues
- 它们允许 HPL 在继续做有用计算的同时轮询面板转发进度
- this is essential for look-ahead overlap
- 这对 look-ahead 的重叠执行至关重要

### 7.4 Trailing update
### 7.4 尾随更新

Main files:
主要文件：

- `src/pgesv/HPL_pdupdateNN.c`
- `src/pgesv/HPL_pdupdateNN.c`
- `src/pgesv/HPL_pdupdateNT.c`
- `src/pgesv/HPL_pdupdateNT.c`
- `src/pgesv/HPL_pdupdateTN.c`
- `src/pgesv/HPL_pdupdateTN.c`
- `src/pgesv/HPL_pdupdateTT.c`
- `src/pgesv/HPL_pdupdateTT.c`
- `src/pgesv/HPL_pdlaswp00*.c`
- `src/pgesv/HPL_pdlaswp00*.c`
- `src/pgesv/HPL_pdlaswp01*.c`
- `src/pgesv/HPL_pdlaswp01*.c`
- `src/pgesv/HPL_spread*.c`
- `src/pgesv/HPL_spread*.c`
- `src/pgesv/HPL_roll*.c`
- `src/pgesv/HPL_roll*.c`

Numerical work:
数值计算部分包括：

- apply row interchanges
- 应用行交换
- form row panel `U`
- 构造行面板 `U`
- triangular solve on `L1`
- 在 `L1` 上做三角求解
- GEMM update on trailing matrix
- 对尾随矩阵进行 GEMM 更新

Communication-heavy subphase:
通信最重的子阶段是：

- `HPL_pdlaswp00*` / `HPL_pdlaswp01*`
- `HPL_pdlaswp00*` / `HPL_pdlaswp01*`

Semantics:
语义：

- redistribute the rows affected by pivots
- 重分配受主元影响的那些行
- construct or spread the row panel `U` where needed
- 在需要的位置构造或扩散行面板 `U`

Implementation style:
实现方式：

- explicit point-to-point exchanges
- 显式点对点交换
- many steps use `HPL_sdrv`
- 很多步骤使用 `HPL_sdrv`
- some steps use nonblocking receive + send (`MPI_Irecv`, `MPI_Send`) inside helper routines like `HPL_roll*`
- 某些步骤在 `HPL_roll*` 这类辅助例程中使用非阻塞接收加发送，如 `MPI_Irecv`、`MPI_Send`

Why not use a standard collective:
为什么不使用标准集合通信：

- ownership after pivoting is irregular
- 主元交换后的数据归属关系是不规则的
- message sizes vary per process row
- 不同进程行的消息大小不同
- some ranks need `U`, others only need specific subsets
- 有些 rank 需要完整的 `U`，有些只需要其中的特定子集
- HPL wants to overlap this phase with panel-forward progress
- HPL 希望把这一阶段和面板转发进度重叠起来

#### `HPL_spread*`
#### `HPL_spread*`

Purpose:
作用：

- spread a locally available portion of `U` to other process rows
- 将本地已有的一部分 `U` 扩散到其他进程行

Implementation:
实现方式：

- custom tree-style point-to-point communication
- 自定义树形点对点通信
- uses MPI derived datatypes to avoid explicit packing when sending strided matrix segments
- 使用 MPI 派生数据类型，在发送跨步矩阵片段时避免显式打包

#### `HPL_roll*`
#### `HPL_roll*`

Purpose:
作用：

- circulate pieces of `U` until every process row has what it needs
- 循环传递 `U` 的各个部分，直到每个进程行都拿到自己需要的数据

Implementation:
实现方式：

- point-to-point ring-like rolling
- 基于点对点的环式滚动传播
- commonly uses `MPI_Irecv` + `MPI_Send` + `MPI_Wait`
- 通常使用 `MPI_Irecv` + `MPI_Send` + `MPI_Wait`

Why:
原因：

- predictable pairwise traffic
- 成对通信流量可预测
- good fit for rolling redistribution of structured row panels
- 很适合对结构化行面板进行滚动式重分发

### 7.5 Upper-triangular solve
### 7.5 上三角求解

Main file:
主要文件：

- `src/pgesv/HPL_pdtrsv.c`
- `src/pgesv/HPL_pdtrsv.c`

Semantics:
语义：

- solve `Ux = y` after LU factorization
- 在 LU 分解之后求解 `Ux = y`

Communication pattern:
通信模式如下：

1. move RHS `b` to the process column owning the last diagonal block
1. 将右端项 `b` 移动到拥有最后一个对角块的进程列
2. propagate solved blocks along `col_comm`
2. 沿 `col_comm` 传播已经求解出的块
3. send partial update contributions along `row_comm`
3. 沿 `row_comm` 发送部分更新贡献
4. replicate final block with a column-wise broadcast
4. 通过按列广播复制最后一个块

Style:
实现风格：

- mostly point-to-point
- 主要是点对点通信
- final replication uses `HPL_broadcast`, which has collective semantics but is still implemented with explicit `MPI_Send` / `MPI_Recv`
- 最后的复制使用 `HPL_broadcast`，它在语义上是集合通信，但底层仍由显式的 `MPI_Send` / `MPI_Recv` 实现

Why:
原因：

- back substitution is latency-bound
- 回代过程本质上受时延限制
- each solved block unlocks the next one
- 每求解出一个块，才会解锁下一个块
- pairwise scheduling reduces unnecessary synchronization
- 成对调度可以减少不必要的同步

### 7.6 Verification and timing
### 7.6 校验与计时

Main file:
主要文件：

- `testing/ptest/HPL_pdtest.c`
- `testing/ptest/HPL_pdtest.c`

Relevant communication:
相关通信包括：

- `HPL_barrier` before timing
- 计时前执行 `HPL_barrier`
- `HPL_all_reduce(..., HPL_max, ...)` for memory allocation checks and `||b||_inf`
- 用 `HPL_all_reduce(..., HPL_max, ...)` 做内存分配检查和 `||b||_inf`
- `HPL_reduce(..., HPL_sum, ...)` for residual accumulation
- 用 `HPL_reduce(..., HPL_sum, ...)` 做残差累加
- `HPL_broadcast` to replicate scalar values needed for error checks
- 用 `HPL_broadcast` 复制误差检查所需的标量值

Important point:
重要的一点是：

Even these "collectives" are usually HPL custom collectives implemented on top of point-to-point communication, except communicator construction and a few direct MPI calls elsewhere in the test harness.
即便这些操作在语义上属于“集合通信”，它们通常也是 HPL 基于点对点通信自实现的集合操作，真正直接使用 MPI 集合调用的主要是通信器构建以及测试框架里少数几个位置。

## 8. Which parts are point-to-point, which parts are collective
## 8. 哪些部分是点对点通信，哪些部分是集合通信

### 8.1 Pure point-to-point communication
### 8.1 纯点对点通信

These code paths are explicitly pairwise:
以下代码路径明确采用成对通信：

- `HPL_send`, `HPL_recv`, `HPL_sdrv`
- `HPL_send`, `HPL_recv`, `HPL_sdrv`
- all row-broadcast backends under `src/comm`
- `src/comm` 下的全部行广播后端
- `HPL_pdmxswp`
- `HPL_pdmxswp`
- `HPL_pdlaswp00*`, `HPL_pdlaswp01*`
- `HPL_pdlaswp00*`, `HPL_pdlaswp01*`
- `HPL_spread*`
- `HPL_spread*`
- `HPL_roll*`
- `HPL_roll*`
- most of `HPL_pdtrsv`
- `HPL_pdtrsv` 的大部分路径

Underlying MPI calls include:
底层 MPI 调用包括：

- `MPI_Send`
- `MPI_Send`
- `MPI_Recv`
- `MPI_Recv`
- `MPI_Irecv`
- `MPI_Irecv`
- `MPI_Ssend`
- `MPI_Ssend`
- `MPI_Issend`
- `MPI_Issend`
- `MPI_Wait`
- `MPI_Wait`
- `MPI_Iprobe`
- `MPI_Iprobe`

### 8.2 Collective semantics implemented by HPL itself
### 8.2 由 HPL 自己实现的集合语义

These have collective meaning, but HPL implements them with pairwise messages:
这些操作在语义上属于集合通信，但 HPL 用成对消息实现了它们：

- `HPL_reduce`
- `HPL_reduce`
- `HPL_broadcast`
- `HPL_broadcast`
- `HPL_all_reduce`
- `HPL_all_reduce`
- `HPL_barrier`
- `HPL_barrier`

Operators used with them:
配套使用的算子包括：

- `HPL_max`
- `HPL_max`
- `HPL_min`
- `HPL_min`
- `HPL_sum`
- `HPL_sum`

Typical uses:
典型用途：

- `HPL_max`
- `HPL_max`

  memory allocation checks, norms, machine constants, timing maxima
  内存分配检查、范数、机器常数以及计时最大值

- `HPL_sum`
- `HPL_sum`

  residual accumulation, norm accumulation
  残差累加、范数累加

- `HPL_min`
- `HPL_min`

  timing minima, machine-constant support paths
  计时最小值、机器常数相关支持路径

### 8.3 True MPI collective or global runtime calls
### 8.3 真正的 MPI 集合调用或全局运行时调用

These are direct MPI library collectives or environment calls:
这些是直接调用 MPI 库或全局运行时的操作：

- `MPI_Init`
- `MPI_Init`
- `MPI_Finalize`
- `MPI_Finalize`
- `MPI_Comm_split`
- `MPI_Comm_split`
- `MPI_Comm_rank`
- `MPI_Comm_rank`
- `MPI_Comm_size`
- `MPI_Comm_size`
- `MPI_Abort`
- `MPI_Abort`
- `MPI_Wtime`
- `MPI_Wtime`

## 9. Why the implementation is designed this way
## 9. 为什么实现会设计成这样

### 9.1 Panel factorization is latency-sensitive
### 9.1 面板分解对时延敏感

The panel is narrow and communication-heavy.
面板很窄，而且通信占比高。

HPL therefore uses:
因此 HPL 采用：

- custom reduction / exchange
- 自定义归约 / 交换
- custom row broadcast
- 自定义行广播
- look-ahead overlap
- look-ahead 重叠执行

instead of leaning on a single generic collective.
而不是依赖单一的通用集合通信。

### 9.2 Trailing update is compute-heavy
### 9.2 尾随更新计算量大

Once the panel information has arrived, the update becomes mostly:
一旦面板信息到位，更新阶段主要就变成了：

- `TRSM`
- `TRSM`
- `GEMM`
- `GEMM`

These are local high-throughput BLAS kernels.
这些都是本地高吞吐的 BLAS 内核。

HPL tries hard to reach this phase quickly and keep it large.
HPL 会尽力尽快进入这一阶段，并尽量让这一阶段保持足够大。

### 9.3 Different networks prefer different panel-broadcast routes
### 9.3 不同网络偏好不同的面板广播路由

The topologies selectable in `HPL.dat` exist because no single route is universally optimal across:
`HPL.dat` 中之所以提供可选拓扑，是因为不存在一种路由对所有系统都普适最优，例如：

- fat-tree systems
- fat-tree 系统
- torus / mesh systems
- torus / mesh 系统
- fabrics with different eager / rendezvous behaviors
- 具有不同 eager / rendezvous 行为的网络
- systems with different bisection bandwidths
- 具有不同二分带宽的系统

### 9.4 HPL wants deterministic message control
### 9.4 HPL 希望获得确定性的消息控制能力

By implementing collectives itself, HPL can control:
通过自己实现集合操作，HPL 可以控制：

- message ids
- 消息编号
- ordering
- 消息顺序
- overlap points
- 重叠点
- panel-specific data layouts
- 面板特定的数据布局
- tuning strategy
- 调优策略

That is a major reason many operations with collective semantics are still coded as custom point-to-point exchanges.
这正是为什么很多具有集合语义的操作最终仍然被写成自定义点对点交换的重要原因。

## 10. A practical reading order for the code
## 10. 一种实用的代码阅读顺序

If you want to read the code from simple to complex, use this order:
如果你希望按从简单到复杂的顺序阅读代码，可以按如下顺序：

1. `testing/ptest/HPL_pddriver.c`
1. `testing/ptest/HPL_pddriver.c`
2. `testing/ptest/HPL_pdtest.c`
2. `testing/ptest/HPL_pdtest.c`
3. `src/pgesv/HPL_pdgesv.c`
3. `src/pgesv/HPL_pdgesv.c`
4. `src/pgesv/HPL_pdgesv0.c`
4. `src/pgesv/HPL_pdgesv0.c`
5. `src/panel/HPL_pdpanel_init.c`
5. `src/panel/HPL_pdpanel_init.c`
6. `src/pfact/HPL_pdfact.c`
6. `src/pfact/HPL_pdfact.c`
7. `src/pfact/HPL_pdmxswp.c`
7. `src/pfact/HPL_pdmxswp.c`
8. `src/comm/HPL_bcast.c` plus one backend such as `src/comm/HPL_1ring.c`
8. `src/comm/HPL_bcast.c`，再加上一个后端实现，例如 `src/comm/HPL_1ring.c`
9. `src/pgesv/HPL_pdupdateNN.c`
9. `src/pgesv/HPL_pdupdateNN.c`
10. `src/pgesv/HPL_pdlaswp00N.c`
10. `src/pgesv/HPL_pdlaswp00N.c`
11. `src/pgesv/HPL_pdtrsv.c`
11. `src/pgesv/HPL_pdtrsv.c`
12. `src/pgesv/HPL_pdgesvK2.c`
12. `src/pgesv/HPL_pdgesvK2.c`

## 11. Final takeaway
## 11. 最终结论

The implementation strategy of HPL in this project can be summarized as:
本项目中的 HPL 实现策略可以概括为：

- use 2D block-cyclic data distribution for load balance
- 使用二维块循环数据分布实现负载均衡
- keep panel work narrow but communication-aware
- 让面板工作保持“小而精”，同时充分考虑通信特性
- realize most communication with custom point-to-point schedules
- 用自定义点对点调度实现大部分通信
- use collective semantics only where they naturally fit, and often still implement them internally with p2p
- 只有在自然适合的地方才采用集合语义，而且其内部也常常仍由点对点实现
- overlap panel communication with trailing BLAS updates through look-ahead
- 通过 look-ahead 让面板通信和尾随 BLAS 更新重叠执行

So, if we describe the project in one sentence:
所以，如果用一句话来描述这个项目：

HPL is not "just LU with MPI"; it is a carefully pipelined distributed LU where the real engineering value lies in how pivoting, panel forwarding, U redistribution, and update overlap are orchestrated.
HPL 并不是“在 MPI 上跑 LU”这么简单；它是一个精心流水化的分布式 LU，实现上的真正工程价值在于如何编排主元处理、面板转发、`U` 的重分发以及更新重叠。
