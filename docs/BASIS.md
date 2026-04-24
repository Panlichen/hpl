# 经典名字

这些名字都来自 LAPACK / BLAS 那套非常经典的命名规则。可以按“前缀 + 矩阵类型 + 操作”来拆。

`gesv`
- `ge` = **general matrix**，一般稠密矩阵
- `sv` = **solve**，求解线性方程组  
- 所以 `gesv` 的意思是：**求解一般稠密矩阵的线性方程组**

`getrf`
- `ge` = general matrix
- `trf` = **triangular factorization**，这里实际指 LU factorization  
- 所以 `getrf` 的意思是：**对一般稠密矩阵做 LU 分解**

`getrs`
- `ge` = general matrix
- `trs` = **triangular solve using factors**  
- 它通常表示：**已经有了 `getrf` 得到的 LU 分解结果后，用这些因子去解方程组**
- 所以常见流程是：
  1. `getrf` 先分解
  2. `getrs` 再求解
  3. `gesv` 则相当于把这两步打包起来

`lamch`
- `la` = LAPACK / linear algebra
- `mch` = **machine constants**
- 所以 `lamch` 的意思是：**查询机器浮点常数**
- 典型返回：
  - `eps`
  - `sfmin`
  - `base`
  - `rmax`
  - `rmin`

再看最前面的数据类型前缀：

- `s` = single，单精度实数
- `d` = double，双精度实数
- `c` = complex，单精度复数
- `z` = double complex，双精度复数

所以：
- `dgesv` = 双精度一般矩阵求解
- `dgetrf` = 双精度一般矩阵 LU 分解
- `dgetrs` = 双精度一般矩阵 LU 回代求解
- `dlamch` = 双精度机器常数查询

**为什么 HPL 看起来不像直接调用 LAPACK，却到处都是 LAPACK 命名风格？**

因为 HPL 和 LAPACK 在“问题定义”上是同一脉络，但在“实现目标”上不一样。

可以这样理解：

- **LAPACK** 主要面向单机/共享内存上的稠密线性代数
- **HPL** 面向分布式内存并行环境下的大规模 LU 求解基准测试

所以 HPL 会保留 LAPACK 的命名语义，让人一看就知道“这个函数在数学上是干什么的”，但实现上往往不是直接调用 LAPACK 那个现成例程，而是自己做了一套更适合分布式并行的版本。

比如你现在看到的这些名字：

- [HPL_pdgesv.c](/home/admin/hpl/src/pgesv/HPL_pdgesv.c)
- [HPL_pdfact.c](/home/admin/hpl/src/pfact/HPL_pdfact.c)
- [HPL_pdlamch.c](/home/admin/hpl/src/pauxil/HPL_pdlamch.c)

这里的额外前缀也有含义：

- `HPL_`：HPL 自己的实现
- `p`：parallel / distributed，并行分布式版本
- `d`：double precision

所以：
- `HPL_pdgesv` 可以理解为 **HPL 的并行双精度一般矩阵求解**
- `HPL_pdlamch` 是 **HPL 的并行双精度机器常数查询**

HPL 不直接照搬 LAPACK，有几个核心原因：

1. **数据分布不同**
- LAPACK 通常假设矩阵在单个进程内
- HPL 的矩阵按 2D block-cyclic 分布在进程网格上

2. **通信必须显式设计**
- HPL 要处理面板广播、行交换、更新传播、回代通信
- 这些都是分布式算法的问题，不是 LAPACK 单机例程能直接覆盖的

3. **HPL 目标是性能基准**
- 它不是单纯“把方程解出来”
- 它还要非常强调通信拓扑、重叠、块大小、递归面板、广播策略等性能因素

4. **保留 LAPACK 风格有认知收益**
- 对做数值线性代数的人来说，`gesv/getrf/getrs/lamch` 这些名字一眼就能看出职责
- 这样 HPL 虽然实现完全并行化了，但语义仍然容易读懂

一句话总结：

**HPL 借用了 LAPACK 的“数学命名体系”，但把具体实现改造成了适合分布式内存和性能测试的并行版本。**

如果你愿意，我可以下一步继续把这几个名字放回 HPL 的真实调用链里，给你画成一条对应关系：

- `pdgesv` 在 HPL 里相当于哪几步
- `getrf/getrs` 的思想分别落在哪些源码文件里
- `pdlamch` 为什么是 `dlamch` 的并行封装版





# LAPACK

LAPACK 的英文全称是 **Linear Algebra PACKage**。

它是一个经典的高性能数值线性代数库，主要用来做这类计算：

- 解线性方程组
- 矩阵分解
  - LU 分解
  - QR 分解
  - Cholesky 分解
  - 特征值分解
  - 奇异值分解
- 最小二乘问题
- 各种矩阵范数、条件数、误差估计

可以把它理解成：

**面向稠密矩阵数值计算的标准基础库。**

它和 BLAS 的关系很紧密：

- **BLAS** 更底层，提供向量/矩阵基本运算，例如 `axpy`、`gemv`、`gemm`
- **LAPACK** 建立在 BLAS 之上，把这些基础算子组织成完整的数值算法

比如：
- `dgemm` 是 BLAS
- `dgetrf`、`dgetrs`、`dlamch` 是 LAPACK

放到你现在这个 HPL 项目里看：

- HPL 本质上是在做一个大规模分布式 LU 求解基准测试
- 它会大量使用 BLAS 风格的局部核
- 同时很多概念、命名和数值判据直接继承自 LAPACK

所以像：

- `HPL_dlamch` 就是 LAPACK `dlamch` 思想/接口的 HPL 版本
- `HPL_pdgesv` 这个名字也明显是在模仿 LAPACK 里 `gesv` 这类“解一般稠密线性方程组”的命名方式

一句话说：

**BLAS 提供“砖块”，LAPACK 提供“搭房子的标准方法”，HPL 则是在并行分布式环境里把这些方法组织成大规模性能测试。**

如果你愿意，我可以下一步继续给你讲：
1. BLAS、LAPACK、ScaLAPACK、HPL 四者之间的关系  
2. 为什么 HPL 看起来不像直接调用 LAPACK，却到处都是 LAPACK 命名风格  
3. `gesv / getrf / getrs / lamch` 这些经典名字分别是什么意思





# BLAS

BLAS 的全称是 **Basic Linear Algebra Subprograms**。

它指的是一组基础线性代数子程序标准，主要提供最常见、最底层的向量和矩阵运算接口，比如：

- 向量加法
- 点积
- 矩阵向量乘
- 矩阵矩阵乘

常见例子有：

- `daxpy`
- `ddot`
- `dgemv`
- `dgemm`

其中前缀里的 `d` 表示 double precision，也就是双精度。

可以把它理解为：

**BLAS 是数值线性代数里的基础算子层。**

很多更高层的库都建立在它上面，比如：

- LAPACK
- ScaLAPACK
- HPL

如果你愿意，我可以继续把 **BLAS / LAPACK / ScaLAPACK / HPL** 的关系给你画成一个很清晰的分层说明。



# 分层说明

可以把它们看成一条从“基础算子”到“分布式基准程序”的技术栈。

**分层关系**

1. **BLAS**
   基础线性代数算子层，提供最底层的高频运算内核。

   典型内容：
   - 向量操作：`axpy`、`dot`
   - 矩阵向量：`gemv`
   - 矩阵矩阵：`gemm`

   它回答的问题是：
   **“最基本的线性代数运算怎么高效做？”**

2. **LAPACK**
   建立在 BLAS 之上的单机稠密线性代数算法库。

   典型内容：
   - `getrf`：LU 分解
   - `getrs`：用 LU 因子解方程
   - `gesv`：直接解线性方程组
   - `geqrf`：QR 分解
   - `syev`：特征值问题

   它回答的问题是：
   **“怎么把 BLAS 这些基础算子组织成完整的数值算法？”**

3. **ScaLAPACK**
   可以理解为 LAPACK 的分布式并行版本，面向分布式内存集群。

   典型特点：
   - 使用 MPI
   - 矩阵按 2D block-cyclic 分布
   - 提供并行 LU、QR、特征值等算法

   它回答的问题是：
   **“如果矩阵分布在很多进程上，LAPACK 那些算法怎么并行化？”**

4. **HPL**
   建立在 BLAS、MPI 和 LAPACK/ScaLAPACK 思想之上的**高性能 Linpack 基准程序**。

   它不是一个通用线性代数库，而是一个专门的 benchmark。
   它主要做一件事：

   **在大规模并行系统上，用双精度 LU 求解一个稠密线性方程组，并测性能。**

   它回答的问题是：
   **“这台超算跑大规模分布式 LU 求解，到底能跑多快？”**

**可以用一张图理解**

```text
HPL
├─ 目标：做高性能 Linpack 基准测试，测并行 LU 求解性能
├─ 依赖：MPI + BLAS
├─ 借鉴：LAPACK / ScaLAPACK 的命名和数值线性代数思想
└─ 特点：专门为 benchmark 调优，不是通用求解库

ScaLAPACK
├─ 目标：分布式并行稠密线性代数库
├─ 依赖：MPI + BLAS + BLACS/PBLAS
└─ 本质：LAPACK 在分布式内存上的扩展

LAPACK
├─ 目标：单机稠密线性代数算法库
├─ 依赖：BLAS
└─ 本质：把基础算子组织成 LU/QR/SVD/特征值等完整算法

BLAS
├─ 目标：基础线性代数运算接口标准
└─ 本质：最底层高频数值计算内核
```

**再换一种“盖楼”式理解**

- **BLAS** 是砖头、钢筋、水泥  
- **LAPACK** 是标准住宅施工图  
- **ScaLAPACK** 是“多工地协同施工版”的施工图  
- **HPL** 不是为了盖各种楼，而是专门办一场比赛：
  **看谁用这套施工体系最快盖出指定的一栋楼**

**它们之间的依赖关系**

- LAPACK 调 BLAS
- ScaLAPACK 调 BLAS，并结合 MPI/BLACS 做分布式计算
- HPL 也大量调 BLAS，并自己实现分布式 LU 流程与通信机制
- HPL 在概念和命名上很像 LAPACK/ScaLAPACK，但实现目标更偏 benchmark 极致性能

**为什么 HPL 和 ScaLAPACK 很像，但又不是一回事**

相同点：
- 都做分布式稠密矩阵计算
- 都使用 2D block-cyclic 分布
- 都涉及 LU、面板分解、广播、更新、回代

不同点：
- **ScaLAPACK** 是通用并行线性代数库
- **HPL** 是面向 Linpack 排名的专用 benchmark
- HPL 会为了性能评测专门设计：
  - 面板广播拓扑
  - 递归 panel factorization
  - 更新重叠
  - swap 策略
  - block size 调优

所以 HPL 往往比通用库更“偏性能工程”。

**结合你现在这个 HPL 项目理解**

在这个代码库里：

- BLAS 角色体现在很多局部核调用上，比如 `dgemv`、`dtrsv`、`dgemm`
- LAPACK 风格体现在命名和数值判据上，比如：
  - `HPL_pdgesv`
  - `HPL_pdtrsv`
  - `HPL_pdlamch`
- ScaLAPACK 风格体现在：
  - 2D 进程网格
  - block-cyclic 分布
  - 面板分解 + 广播 + 更新 的并行 LU 结构
- HPL 自己真正做的是：
  - 把这些思想组织成一条专门为性能测试优化的分布式 LU 流水线

**一句话总结**

**BLAS 是算子层，LAPACK 是单机算法层，ScaLAPACK 是分布式算法库层，HPL 是建立在这些思想和基础设施之上的高性能 Linpack 基准程序。**

如果你愿意，我下一步可以继续给你画一张更贴近你当前代码仓的“源码映射版分层图”，把这四层直接对应到这个项目里的目录，例如：
- `src/blas-like calls`
- `src/pgesv`
- `src/pfact`
- `src/comm`
- `src/grid`
- `testing/ptest`









