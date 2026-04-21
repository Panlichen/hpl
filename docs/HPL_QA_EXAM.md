# HPL Q&A And Exam-Style Questions
# HPL 答疑与考试风格问答

## 1. Basic understanding
## 1. 基础理解

### Q1. What is the entry point of one HPL test case in this project?
### Q1. 在本项目中，一个 HPL 测试用例的入口是什么？

**Answer / 答案**

The outermost program entry is `testing/ptest/HPL_pddriver.c`.
最外层的程序入口是 `testing/ptest/HPL_pddriver.c`。

It reads `HPL.dat`, enumerates parameter combinations, creates the process grid, selects algorithm function pointers, and calls `HPL_pdtest` for each case.
它读取 `HPL.dat`，枚举参数组合，创建进程网格，选择算法函数指针，并针对每个测试组合调用 `HPL_pdtest`。

### Q2. Which function really launches the distributed solve?
### Q2. 真正启动分布式求解的是哪个函数？

**Answer / 答案**

`testing/ptest/HPL_pdtest.c` calls `HPL_pdgesv`, which is the main distributed solve routine.
`testing/ptest/HPL_pdtest.c` 会调用 `HPL_pdgesv`，它才是主要的分布式求解例程。

### Q3. What are the two main solve schedules chosen by `HPL_pdgesv`?
### Q3. `HPL_pdgesv` 会选择哪两种主要的求解调度方式？

**Answer / 答案**

- `HPL_pdgesv0`: right-looking LU without look-ahead
- `HPL_pdgesv0`：不带 look-ahead 的 right-looking LU
- `HPL_pdgesvK2`: right-looking LU with look-ahead
- `HPL_pdgesvK2`：带 look-ahead 的 right-looking LU

The selection depends mainly on `ALGO->depth` and on whether there is more than one process column.
具体选择主要取决于 `ALGO->depth`，以及进程列数是否大于 1。

## 2. Process-grid and data distribution
## 2. 进程网格与数据分布

### Q4. Why does HPL create `all_comm`, `row_comm`, and `col_comm`?
### Q4. 为什么 HPL 要创建 `all_comm`、`row_comm` 和 `col_comm`？

**Answer / 答案**

- `all_comm` serves whole-grid collectives such as timing aggregation and global checks.
- `all_comm` 用于整个进程网格上的集合操作，例如计时聚合和全局检查。
- `row_comm` serves row-wise panel broadcast and row-wise scalar propagation.
- `row_comm` 用于按进程行进行的面板广播和标量传播。
- `col_comm` serves pivot search, pivot-row movement, `U` redistribution, and parts of triangular solve.
- `col_comm` 用于主元搜索、主元行移动、`U` 的重分发，以及上三角求解中的部分步骤。

This layered design reduces unnecessary participation in each communication phase.
这种分层设计可以减少各个通信阶段中不必要的进程参与。

### Q5. Why does HPL use 2D block-cyclic distribution?
### Q5. 为什么 HPL 使用二维块循环分布？

**Answer / 答案**

Because it balances work and memory across ranks, preserves large local BLAS kernels, and avoids severe ownership imbalance as the factorization progresses.
因为它能在各个 rank 之间平衡计算和内存，占据更大规模的本地 BLAS 内核，并避免随着分解推进而出现严重的数据归属失衡。

## 3. Panel factorization
## 3. 面板分解

### Q6. During panel factorization, which process column performs the real factorization work?
### Q6. 在面板分解期间，真正执行分解工作的是哪一列进程？

**Answer / 答案**

Only the process column owning the current panel performs the panel factorization.
只有拥有当前面板的那一列进程会真正执行面板分解。

Other process columns obtain the panel data later from the panel broadcast.
其他进程列稍后通过面板广播获得相关面板数据。

### Q7. Which function finds the local pivot candidate?
### Q7. 哪个函数负责寻找局部主元候选？

**Answer / 答案**

`HPL_dlocmax` finds the local maximum entry in the current column segment and packs its metadata into workspace.
`HPL_dlocmax` 会在当前列段中找到本地主元候选，并将其元数据打包到工作区中。

### Q8. Which function performs global pivot-row selection and movement?
### Q8. 哪个函数负责执行全局主元行选择和主元行移动？

**Answer / 答案**

`HPL_pdmxswp`.
`HPL_pdmxswp`。

### Q9. Is `HPL_pdmxswp` implemented with MPI collective calls?
### Q9. `HPL_pdmxswp` 是通过 MPI 集合通信实现的吗？

**Answer / 答案**

No.
不是。

It is implemented with point-to-point communication using wrappers such as `HPL_send`, `HPL_recv`, and `HPL_sdrv`.
它是通过点对点通信实现的，使用了 `HPL_send`、`HPL_recv` 和 `HPL_sdrv` 等封装。

### Q10. Why does HPL not simply use `MPI_Reduce` followed by `MPI_Bcast` in `HPL_pdmxswp`?
### Q10. 为什么 HPL 在 `HPL_pdmxswp` 中不直接使用 `MPI_Reduce` 再接 `MPI_Bcast`？

**Answer / 答案**

Because HPL must move not only pivot metadata but also the winning pivot row itself.
因为 HPL 不仅要传递主元元数据，还必须传递获胜的主元行本身。

Fusing max selection and row movement into one custom exchange reduces synchronization and startup overhead.
把最大值选择和主元行移动融合进一次自定义交换中，可以减少同步和消息启动开销。

## 4. Panel broadcast
## 4. 面板广播

### Q11. What is the purpose of `HPL_binit`, `HPL_bcast`, and `HPL_bwait`?
### Q11. `HPL_binit`、`HPL_bcast` 和 `HPL_bwait` 的作用是什么？

**Answer / 答案**

They manage row-wise broadcast of the current panel from the owner process column across the process row.
它们负责管理当前面板从拥有该面板的进程列沿整条进程行进行的广播。

### Q12. Does `HPL_bcast` directly call `MPI_Bcast`?
### Q12. `HPL_bcast` 是否直接调用 `MPI_Bcast`？

**Answer / 答案**

No.
不是。

It dispatches to a custom topology backend such as:
它会分派到某个自定义拓扑后端，例如：

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

These are implemented with point-to-point MPI.
这些后端都是基于点对点 MPI 实现的。

### Q13. Why are there multiple panel-broadcast topologies?
### Q13. 为什么面板广播会有多种拓扑？

**Answer / 答案**

Because different systems and networks prefer different communication routes.
因为不同系统和网络更适合不同的通信路由。

HPL exposes the choice as a tuning parameter.
HPL 把这种选择暴露为一个可调参数。

## 5. Trailing update
## 5. 尾随更新

### Q14. What are the main numerical operations in a trailing update?
### Q14. 尾随更新中的主要数值操作有哪些？

**Answer / 答案**

- apply row interchanges
- 应用行交换
- solve for the row panel `U` using `TRSM`
- 使用 `TRSM` 求解行面板 `U`
- update the trailing matrix using `GEMM`
- 使用 `GEMM` 更新尾随矩阵

### Q15. Which family of functions performs row swapping and `U` redistribution?
### Q15. 哪一类函数负责行交换和 `U` 的重分发？

**Answer / 答案**

- `HPL_pdlaswp00*`
- `HPL_pdlaswp00*`
- `HPL_pdlaswp01*`
- `HPL_pdlaswp01*`
- `HPL_spread*`
- `HPL_spread*`
- `HPL_roll*`
- `HPL_roll*`

### Q16. Why is `U` redistribution communication-heavy?
### Q16. 为什么 `U` 的重分发通信负担很重？

**Answer / 答案**

Because pivoting changes ownership irregularly.
因为主元交换会以不规则方式改变数据归属。

Different process rows need different subsets of rows, so the data movement pattern is sparse and data-dependent.
不同进程行需要不同行的子集，因此数据移动模式既稀疏又依赖于实际数据。

### Q17. Why does HPL compute parts of `U` redundantly?
### Q17. 为什么 HPL 会冗余地计算 `U` 的一部分？

**Answer / 答案**

To reduce synchronization.
是为了减少同步。

Redundant local computation is cheaper than forcing another global communication step before every trailing update.
与其在每次尾随更新前再强制进行一次全局通信，不如做一些冗余的本地计算，代价更低。

## 6. Look-ahead
## 6. Look-ahead

### Q18. What problem does look-ahead solve in HPL?
### Q18. Look-ahead 在 HPL 中解决了什么问题？

**Answer / 答案**

It overlaps panel factorization and communication with updates from previous panels, reducing idle time caused by the latency-sensitive panel path.
它把当前面板分解及其通信，与前面板的更新过程重叠起来，从而减少由面板路径时延敏感所造成的空闲时间。

### Q19. In `HPL_pdgesvK2`, what is the role of the `panel[]` array?
### Q19. 在 `HPL_pdgesvK2` 中，`panel[]` 数组起什么作用？

**Answer / 答案**

It forms a software pipeline of panel descriptors.
它构成了一个面板描述符的软件流水线。

Different entries correspond to panels at different pipeline stages such as factored, broadcast, partially updated, or ready to recycle.
其中不同的数组项对应流水线不同阶段的面板，例如已分解、已广播、部分更新完成，或者即将被回收复用。

### Q20. Why is look-ahead particularly useful for HPL?
### Q20. 为什么 look-ahead 对 HPL 特别有用？

**Answer / 答案**

Because panel factorization is communication- and latency-bound, while trailing matrix update is compute-bound.
因为面板分解通常受通信和时延限制，而尾随矩阵更新则主要受计算能力限制。

Overlap lets the expensive BLAS update hide panel communication cost.
重叠执行可以让高代价的 BLAS 更新隐藏面板通信开销。

## 7. Triangular solve
## 7. 上三角求解

### Q21. Which function solves the upper-triangular system after LU?
### Q21. LU 分解完成后，哪个函数负责求解上三角系统？

**Answer / 答案**

`HPL_pdtrsv`.
`HPL_pdtrsv`。

### Q22. What is the first communication step in `HPL_pdtrsv`?
### Q22. `HPL_pdtrsv` 中的第一步通信是什么？

**Answer / 答案**

It moves the right-hand side `b` to the process column that owns the last diagonal block.
它会先把右端项 `b` 移动到拥有最后一个对角块的进程列。

### Q23. Is the triangular solve dominated by collectives or point-to-point communication?
### Q23. 上三角求解阶段主要由集合通信主导，还是由点对点通信主导？

**Answer / 答案**

Mostly by point-to-point communication.
主要由点对点通信主导。

The final solution-block replication uses collective semantics through `HPL_broadcast`, but that wrapper itself is still implemented with pairwise sends and receives.
最后一个解块的复制虽然通过 `HPL_broadcast` 体现了集合语义，但这个封装本身仍然是用成对的发送和接收实现的。

### Q24. Why is the triangular solve said to be latency-bound?
### Q24. 为什么说上三角求解是受时延限制的？

**Answer / 答案**

Because the solve advances block by block, and each newly solved block unlocks the next step.
因为求解是按块逐步推进的，而每个新求出的块都会决定下一步能否继续。

This creates a sequential dependency chain where message startup latency matters more than bulk bandwidth.
这会形成一条顺序依赖链，在这种情况下，消息启动时延往往比大带宽更关键。

## 8. Collectives and operators
## 8. 集合操作与算子

### Q25. Name the HPL routines that provide collective semantics.
### Q25. 请列出提供集合语义的 HPL 例程。

**Answer / 答案**

- `HPL_reduce`
- `HPL_reduce`
- `HPL_broadcast`
- `HPL_broadcast`
- `HPL_all_reduce`
- `HPL_all_reduce`
- `HPL_barrier`
- `HPL_barrier`

### Q26. Are these routines necessarily implemented with MPI collectives?
### Q26. 这些例程一定是用 MPI 集合通信实现的吗？

**Answer / 答案**

No.
不是。

In this project they are mostly implemented with explicit point-to-point communication.
在本项目中，它们大多是通过显式点对点通信实现的。

### Q27. Which reduction operators appear in these HPL collectives?
### Q27. 这些 HPL 集合操作中会出现哪些归约算子？

**Answer / 答案**

- `HPL_max`
- `HPL_max`
- `HPL_min`
- `HPL_min`
- `HPL_sum`
- `HPL_sum`

### Q28. Give one typical usage of `HPL_max`.
### Q28. 请给出一个 `HPL_max` 的典型用途。

**Answer / 答案**

It is used for:
它通常用于：

- memory-allocation failure aggregation
- 内存分配失败的聚合
- timing maxima
- 计时最大值统计
- norm maxima
- 范数最大值统计
- machine-constant support paths
- 机器常数相关支持路径

### Q29. Give one typical usage of `HPL_sum`.
### Q29. 请给出一个 `HPL_sum` 的典型用途。

**Answer / 答案**

It is used to accumulate distributed residual vectors or norm contributions.
它常用于累加分布式残差向量或范数贡献。

### Q30. Why might HPL prefer self-implemented collectives over direct MPI collectives?
### Q30. 为什么 HPL 可能更偏向于自实现集合通信，而不是直接使用 MPI 集合通信？

**Answer / 答案**

Because HPL wants explicit control over:
因为 HPL 希望显式控制以下内容：

- message ids
- 消息编号
- schedule shape
- 调度形状
- overlap points
- 重叠点
- custom payload layout
- 自定义负载布局
- tuning behavior across systems
- 跨系统的调优行为

## 9. Applied analysis
## 9. 应用分析

### Q31. Which phase is the most communication-sensitive in HPL, and why?
### Q31. HPL 中哪个阶段对通信最敏感，为什么？

**Answer / 答案**

Panel factorization is usually the most communication-sensitive phase because the panel is narrow, the amount of floating-point work is limited, and pivoting requires frequent synchronization and data movement.
面板分解通常是对通信最敏感的阶段，因为面板很窄，浮点计算量有限，而主元选择又要求频繁同步和数据移动。

### Q32. Which phase contributes most of the floating-point throughput?
### Q32. 哪个阶段贡献了主要的浮点吞吐？

**Answer / 答案**

The trailing matrix update, mainly through `GEMM`.
尾随矩阵更新阶段贡献了主要吞吐，核心来源是 `GEMM`。

### Q33. Why is the panel broadcast done across process rows rather than process columns?
### Q33. 为什么面板广播是沿进程行做，而不是沿进程列做？

**Answer / 答案**

Because once a panel is factorized in its owner process column, the information it contains is needed by all process columns that own pieces of the trailing matrix in the same process row.
因为一旦某个面板在其所属进程列中完成分解，该面板中的信息就会被同一进程行上、拥有尾随矩阵块的所有进程列所需要。

### Q34. Why is pivot search performed inside `col_comm`?
### Q34. 为什么主元搜索是在 `col_comm` 内完成的？

**Answer / 答案**

Because pivot candidates for a given panel column are distributed across process rows within the panel-owning process column.
因为对于某个面板列而言，它的主元候选分布在拥有该面板的那一列进程中的各个进程行上。

### Q35. Why is the update stage a mix of communication and local BLAS?
### Q35. 为什么更新阶段会同时包含通信和本地 BLAS？

**Answer / 答案**

Because HPL must first redistribute pivot-affected rows and form `U`, but once that data is in place the heavy numerical update is local `TRSM + GEMM`.
因为 HPL 必须先重分发受主元影响的行并构造 `U`，而一旦这些数据到位，后续的大规模数值更新就可以变成纯本地的 `TRSM + GEMM`。

## 10. Short exam section
## 10. 简答考试题

### Q36. Distinguish "collective semantics" from "MPI collective implementation" in this project.
### Q36. 请区分本项目中的“集合语义”和“MPI 集合实现”。

**Answer / 答案**

"Collective semantics" means the operation logically involves a group, such as broadcast or reduction.
“集合语义”指的是该操作在逻辑上面向一个进程组，例如广播或归约。

"MPI collective implementation" means the code directly calls routines like `MPI_Bcast` or `MPI_Reduce`.
“MPI 集合实现”指的是代码直接调用了 `MPI_Bcast` 或 `MPI_Reduce` 这类 MPI 集合例程。

In this project, many operations have collective semantics but are implemented with custom point-to-point schedules.
在本项目中，许多操作虽然具有集合语义，但其实现方式却是自定义的点对点调度。

### Q37. Explain in one paragraph why HPL uses custom point-to-point communication so heavily.
### Q37. 请用一段话解释为什么 HPL 如此大量地使用自定义点对点通信。

**Answer / 答案**

HPL is a benchmark whose performance depends on precise control of panel movement, pivot-row redistribution, overlap, and message routing.
HPL 是一个对性能极其敏感的基准程序，它的表现依赖于对面板移动、主元行重分发、重叠执行以及消息路由的精确控制。

Standard collectives do not always match the required payload layout or the desired overlap behavior, so HPL often implements the same logical operation through tuned point-to-point schedules.
标准集合通信并不总能匹配所需的数据布局和期望的重叠行为，因此 HPL 往往通过经过调优的点对点调度来实现同样的逻辑操作。

### Q38. If network latency becomes much worse, which HPL phases are likely to suffer first?
### Q38. 如果网络时延显著变差，HPL 的哪些阶段最先受到影响？

**Answer / 答案**

Panel factorization, panel broadcast, and triangular solve will suffer first, because they have more fine-grained and dependency-driven communication than the large BLAS-based trailing update.
最先受到影响的通常是面板分解、面板广播和上三角求解，因为与大规模 BLAS 主导的尾随更新相比，这几个阶段的通信更细粒度、更依赖时序。

### Q39. If GEMM becomes faster but network performance stays the same, what becomes more visible?
### Q39. 如果 GEMM 更快了，但网络性能不变，什么开销会变得更显眼？

**Answer / 答案**

The communication cost of the panel path becomes more visible, especially pivot handling, panel broadcast, and synchronization around look-ahead progress.
面板路径上的通信开销会变得更加显眼，尤其是主元处理、面板广播以及 look-ahead 进度相关的同步。

### Q40. In one sentence, summarize the implementation philosophy of HPL.
### Q40. 请用一句话总结 HPL 的实现哲学。

**Answer / 答案**

HPL organizes distributed LU so that communication-heavy panel work is tightly controlled and overlapped, while the bulk of the runtime is pushed into large local BLAS updates.
HPL 的核心思想是把通信密集的面板工作做严格控制并尽量重叠执行，同时把绝大多数运行时间推到大规模本地 BLAS 更新上去。
