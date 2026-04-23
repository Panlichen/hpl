# WWW 文档阅读指南与考卷

## 0. 基础知识复习

1. 高斯消元法本质上是**LU 分解**的实现：  
   $$
   PA = LU
   $$
   其中 \(L\) 为单位下三角（消元因子 \(m_{ik}\) 的相反数存放于下三角），\(U\) 为上三角，\(P\) 为置换矩阵（记录行交换）。求解 \(Ax=b\) 变为：

   1. 解 \(Ly = Pb\) （前代）
   2. 解 \(Ux = y\) （回代）

   > $P$是记录寻找主元的时候进行的行交换

将消元过程直接记录在矩阵中，可以得到一种无需额外存储乘数的紧凑表达方式。设原始矩阵 \(A\) 经过消元后变为 \(U\)，同时将乘数 \(l_{ik}\) 存储在 \(A\) 的下三角部分（即 \(a_{ik}^{(k)}\) 的位置），最终得到：
$$
A = 
\begin{bmatrix}
u_{11} & u_{12} & \cdots & u_{1n} \\
l_{21} & u_{22} & \cdots & u_{2n} \\
\vdots & \vdots & \ddots & \vdots \\
l_{n1} & l_{n2} & \cdots & u_{nn}
\end{bmatrix}
$$
其中 \(U\) 的元素 \(u_{ij}\) 是最终上三角矩阵的元素，\(L\) 的元素 \(l_{ij}\)（\(i > j\)）是消元过程中记录的乘数，且 \(l_{ii}=1\)。这种==紧凑存储方式==正是LU分解的常用实现。

> [!NOTE]
>
> LU分解的下三角部分其实存的就是当前行与主元行的数的比值，不额外加负号。至于是否称这个数为消元因子的相反数，看具体怎么定义消元因子，不是很重要

## 0. 明确：panel的定义

应该是主要指宽度不超过块大小`nb`的一个“长条”，通常指多个列，row panel就指多个行



准确地说，在这个 HPL 项目里，**“panel”默认指当前迭代要处理的那一块“列面板”**：

**它是当前 trailing submatrix ==最左侧==的一个宽度为 `jb`（通常不超过块大小 `nb`）的==列块==，覆盖当前==剩余的全部行==；这一块先在其所属的进程列上做面板分解，然后作为后续广播、交换、更新的核心数据对象被封装和传递。**

最直接的文档佐证在 [algorithm.html](/home/admin/hpl/www/algorithm.html#L69)：
- “at each iteration of the loop a panel of `nb` columns is factorized”
- 这说明 panel 是**每轮主循环处理的列块**，宽度就是 `nb`。
- 紧接着 [algorithm.html](/home/admin/hpl/www/algorithm.html#L83) 又说每次 panel factorization “occurs in one column of processes”，说明它的**计算归属是一个进程列**。
- 在 [algorithm.html](/home/admin/hpl/www/algorithm.html#L105) 还明确写了 “this panel of columns is broadcast to the other process columns”，说明它不是普通局部块，而是**要被广播的列面板**。

源码里的定义更精确。`HPL_pdfact` 的注释直接写明：`HPL_pdfact recursively factorizes a 1-dimensional panel of columns`，也就是“递归分解一个一维的列面板” [HPL_pdfact.c](/home/admin/hpl/src/pfact/HPL_pdfact.c#L67)。而在主循环里，`HPL_pdgesv0` 也把一次迭代拆成四步：重建 panel 描述符、分解 panel、广播 panel、更新 trailing matrix [HPL_pdgesv0.c](/home/admin/hpl/src/pgesv/HPL_pdgesv0.c#L124)。这说明 panel 是**算法流程中的一级工作单元**，不是单纯一个缓冲区名字。

再往下看数据结构，`HPL_T_panel` 把 panel 明确建模成“当前面板 + 通信/更新所需状态”的描述符 [hpl_panel.h](/home/admin/hpl/include/hpl_panel.h#L60)：
- `A` 指向当前 trailing part of A
- `jb` 是 panel width
- `m/n/ia/ja` 给出当前 trailing 子问题的全局尺寸和起点
- `L2/L1/DPIV/DINFO/U` 则保存面板分解及后续更新/通信需要的数据
- `buffers/counts/dtypes/request/status/msgid/len` 则说明 panel 还是**广播载荷的组织单位**

`HPL_pdpanel_init` 也把这个定义说得很直白：`M` 是 panel 的全局行数，`JB` 是 panel 的列数，`IA/JA` 标识 panel 和 trailing submatrix 的全局起点 [HPL_pdpanel_init.c](/home/admin/hpl/src/panel/HPL_pdpanel_init.c#L105)。并且它把 `L2 + L1 + DPIV + DINFO` 组织成一个逻辑载荷，专门服务后续 panel broadcast [HPL_pdpanel_init.c](/home/admin/hpl/src/panel/HPL_pdpanel_init.c#L191)。`HPL_packL` 进一步说明自己是在“forms the MPI data type for the panel to be broadcast” [HPL_packL.c](/home/admin/hpl/src/comm/HPL_packL.c#L73)。

所以，如果要给出一个适合答辩/考试的定义，我建议你这样说：

**Panel 是 HPL 在一次 LU 主循环迭代中选出的当前列块；它是当前尾随子矩阵最左侧、宽度为 `jb` 的列面板，先在所属进程列上完成主元搜索与面板分解，再被打包成 `HPL_T_panel` 所描述的状态对象，用于行向广播、行交换、生成 `U` 行面板以及尾随子矩阵更新。**

最后补一个容易混淆但很重要的点：  
在更新阶段，文档还会说到 **row panel `U`**，例如 [algorithm.html](/home/admin/hpl/www/algorithm.html#L203) 的 “current row panel U”。这说明 HPL 里有“列面板”和“行面板 U”两个相关概念；但**单独说 panel 时，默认是列面板**，`U` 是由该列面板分解结果派生出来、供更新使用的行面板。

如果你愿意，我可以下一步把这个定义再整理成：
1. “论文/汇报版表述”  
2. “面试/考试标准答案版”  
3. “结合通信过程的扩展版定义”

## 1. 阅读目标

阅读 `www/` 下的 HPL 文档时，不建议把它当成普通 API 手册来顺序翻页。

更高效的方式是：

1. 先建立“整体算法流程”的脑图。
2. 再把每个文档页面映射到真实源码位置。
3. 最后重点追踪通信路径，区分点对点通信与集合语义通信。

如果你的目标是真正吃透这套代码，建议始终按“网页文档 + 源码”双线并行阅读。

## 2. 推荐阅读顺序

### 第一层：建立全局认识

优先顺序：

1. `www/index.html`
2. `www/algorithm.html`
3. `www/documentation.html`
4. `www/tuning.html`

阅读目标：

- `index.html`
  先理解 HPL 的目标、所求解的问题类型，以及它的核心关键词。
- `algorithm.html`
  这是最重要的一页，要把主循环、面板分解、面板广播、look-ahead、更新、回代串起来。
- `documentation.html`
  用它建立“网页文档页面”与“源码函数族”之间的目录关系。
- `tuning.html`
  把 `HPL.dat` 中的参数和具体代码路径对应起来。

### 第二层：建立执行主线

优先顺序：

1. `www/HPL_pddriver.html`
2. `www/HPL_pdtest.html`
3. `www/HPL_pdgesv.html`
4. `www/HPL_pdgesv0.html`
5. `www/HPL_pdgesvK2.html`
6. `www/HPL_pdtrsv.html`

建议同时打开的源码：

- `testing/ptest/HPL_pddriver.c`
- `testing/ptest/HPL_pdtest.c`
- `src/pgesv/HPL_pdgesv.c`
- `src/pgesv/HPL_pdgesv0.c`
- `src/pgesv/HPL_pdgesvK2.c`
- `src/pgesv/HPL_pdtrsv.c`

阅读目标：

- 理解一次完整 HPL 运行是如何从测试驱动走到分布式 LU 分解，再走到最后的回代与校验。
- 区分无 look-ahead 与有 look-ahead 的调度差异。

### 第三层：深入面板分解

优先顺序：

1. `www/HPL_pdfact.html`
2. `www/HPL_pdmxswp.html`
3. `www/HPL_dlocmax.html`
4. `www/HPL_pdpanllN.html` / `www/HPL_pdpancrN.html` / `www/HPL_pdpanrlN.html`
5. 对应的递归版本 `www/HPL_pdrpan*.html`

建议同时打开的源码：

- `src/pfact/HPL_pdfact.c`
- `src/pfact/HPL_pdmxswp.c`
- `src/pfact/HPL_dlocmax.c`
- `src/pfact/HPL_pdpan*.c`
- `src/pfact/HPL_pdrpan*.c`

阅读目标：

- 理解主元搜索如何发生。
- 理解为什么 `swap + broadcast` 会被融合到同一个通信步骤中。
- 理解面板分解为什么是整个算法的关键路径。

### 第四层：深入广播与通信基础设施

优先顺序：

1. `www/HPL_bcast.html`
2. `www/HPL_binit.html`
3. `www/HPL_bwait.html`
4. `www/HPL_broadcast.html`
5. `www/HPL_reduce.html`
6. `www/HPL_all_reduce.html`
7. `www/HPL_barrier.html`
8. `www/HPL_send.html`
9. `www/HPL_recv.html`
10. `www/HPL_sdrv.html`

建议同时打开的源码：

- `src/comm/HPL_bcast.c`
- `src/comm/HPL_binit.c`
- `src/comm/HPL_bwait.c`
- `src/grid/HPL_broadcast.c`
- `src/grid/HPL_reduce.c`
- `src/grid/HPL_all_reduce.c`
- `src/grid/HPL_barrier.c`
- `src/comm/HPL_send.c`
- `src/comm/HPL_recv.c`
- `src/comm/HPL_sdrv.c`

阅读目标：

- 区分两类“广播”：
  - 面板广播 `HPL_bcast`：语义上是广播，但走的是自定义拓扑。
  - 网格广播 `HPL_broadcast`：语义上是集合通信，但底层由点对点消息实现。
- 区分两类“归约”：
  - 自定义逻辑的面板内主元竞争。
  - HPL 通用集合基础设施 `HPL_reduce` / `HPL_all_reduce`。

### 第五层：深入更新路径

优先顺序：

1. `www/HPL_pdupdateNN.html`
2. `www/HPL_pdupdateNT.html`
3. `www/HPL_pdupdateTN.html`
4. `www/HPL_pdupdateTT.html`
5. `www/HPL_pdlaswp00N.html`
6. `www/HPL_pdlaswp01N.html`
7. `www/HPL_rollN.html`
8. `www/HPL_spreadN.html`

建议同时打开的源码：

- `src/pgesv/HPL_pdupdateNN.c`
- `src/pgesv/HPL_pdupdateNT.c`
- `src/pgesv/HPL_pdupdateTN.c`
- `src/pgesv/HPL_pdupdateTT.c`
- `src/pgesv/HPL_pdlaswp00N.c`
- `src/pgesv/HPL_pdlaswp01N.c`
- `src/pgesv/HPL_rollN.c`
- `src/pgesv/HPL_spreadN.c`

阅读目标：

- 理解为什么更新阶段既有通信又有大量本地 BLAS。
- 理解行面板 `U` 是怎样构造、传播、滚动和重分发的。
- 理解为什么这里不能简单地用标准集合通信一把做完。

## 3. 推荐阅读方式

### 方式一：按算法主线读

适合第一次系统梳理。

步骤：

1. 先读 `index.html` 和 `algorithm.html`。
2. 再读 `HPL_pddriver -> HPL_pdtest -> HPL_pdgesv -> HPL_pdgesv0 / HPL_pdgesvK2`。
3. 然后进入 `pdfact -> pdmxswp -> bcast -> pdupdate -> pdtrsv`。

优点：

- 不容易迷失在大量函数页面里。
- 能最快建立“程序如何跑起来”的整体认知。

### 方式二：按通信主线读

适合重点研究并行通信设计。

建议聚焦以下页面：

1. `algorithm.html`
2. `HPL_pdmxswp.html`
3. `HPL_bcast.html`
4. `HPL_broadcast.html`
5. `HPL_reduce.html`
6. `HPL_all_reduce.html`
7. `HPL_pdlaswp00N.html`
8. `HPL_rollN.html`
9. `HPL_spreadN.html`
10. `HPL_pdtrsv.html`

优点：

- 能快速看清整个系统里最关键的通信模式。
- 特别适合做超算应用分析、汇报、考试复习。

### 方式三：按参数调优读

适合理解 `HPL.dat` 与代码实现之间的关系。

建议顺序：

1. `tuning.html`
2. `HPL_pdgesv0.html`
3. `HPL_pdgesvK2.html`
4. `HPL_pdfact.html`
5. `HPL_bcast.html`
6. `HPL_pdlaswp00N.html`

重点关注：

- `PFACT` / `RFACT`
- `NBMIN`
- `NDIV`
- `BCAST`
- `DEPTH`
- `SWAP`

## 4. 通信阅读重点

### 4.1 哪些地方主要是点对点通信

重点页面：

- `HPL_pdmxswp.html`
- `HPL_bcast.html`
- `HPL_binit.html`
- `HPL_bwait.html`
- `HPL_send.html`
- `HPL_recv.html`
- `HPL_sdrv.html`
- `HPL_pdlaswp00N.html`
- `HPL_pdlaswp01N.html`
- `HPL_rollN.html`
- `HPL_spreadN.html`
- `HPL_pdtrsv.html`

对应特征：

- 使用明确的发送方和接收方。
- 经常出现环、二叉交换、树形传播、滚动传播等拓扑。
- 更关注时延、消息启动开销和路径控制。

### 4.2 哪些地方具有集合语义

重点页面：

- `HPL_broadcast.html`
- `HPL_reduce.html`
- `HPL_all_reduce.html`
- `HPL_barrier.html`

要注意：

这些页面描述的是集合语义，不等于底层一定调用了 MPI 原生集合通信。

在 HPL 中，它们很多是：

- 逻辑上：broadcast / reduce / all-reduce / barrier
- 实现上：HPL 自己基于点对点消息搭建

### 4.3 具体使用了什么算子

重点算子：

- `HPL_max`
- `HPL_min`
- `HPL_sum`

常见用途：

- `HPL_max`
  用于范数、分配状态检查、机器常数支持路径、计时上界等。
- `HPL_sum`
  用于残差累加、局部贡献汇总。
- `HPL_min`
  用于部分计时与辅助统计场景。

### 4.4 为什么要这么设计

核心原因：

1. 面板路径是关键路径，必须把时延压低。
2. 尾随更新计算量大，适合与通信重叠。
3. HPL 希望精确控制消息编号、拓扑、重叠点与数据布局。
4. 标准集合通信未必最适合主元行传播、`U` 重分发这种强数据依赖场景。

## 5. 考卷与答疑

### 一、基础题

#### 1. HPL 的主执行入口链路是什么？

答：

主执行链路可以概括为：

- `HPL_pddriver`
- `HPL_pdtest`
- `HPL_pdgesv`
- `HPL_pdgesv0` 或 `HPL_pdgesvK2`
- `HPL_pdtrsv`

其中 `HPL_pddriver` 负责参数枚举和测试驱动，`HPL_pdtest` 负责数据生成、调用求解与校验，`HPL_pdgesv*` 负责 LU 主流程，`HPL_pdtrsv` 负责最终回代。

#### 2. 为什么 `algorithm.html` 是最关键的网页文档？

答：

因为它把 HPL 的核心算法流程串成了一条主线，包括：

- 主循环
- 面板分解
- 面板广播
- look-ahead
- 更新
- 回代
- 校验

它是把网页文档和源码结构连起来的最佳入口。

### 二、算法流程题

#### 3. HPL 的主循环为什么选择 right-looking 变体？

答：

因为 right-looking 变体便于把当前面板分解完成后，立即把工作重心转到尾随矩阵更新上，而尾随矩阵更新可以充分利用大规模 BLAS-3 计算，从而提升总体性能。

#### 4. 无 look-ahead 与有 look-ahead 的本质差异是什么？

答：

- 无 look-ahead：当前面板分解、广播、更新基本按顺序执行。
- 有 look-ahead：当前面板分解与下一面板广播、之前面板更新之间可以重叠。

本质上，有 look-ahead 是为了让通信密集的面板路径与计算密集的更新路径并行推进。

#### 5. 为什么面板分解是关键路径？

答：

因为面板很窄，计算量有限，但包含频繁的主元搜索、行交换和传播，通常更受时延和同步影响，难以像 GEMM 一样靠大算量摊平通信成本。

### 三、通信题

#### 6. `HPL_pdmxswp` 属于点对点通信还是集合通信？

答：

它属于点对点通信。

但从逻辑效果上看，它实现的是一个“主元竞争 + 主元行传播”的融合操作，具有类似 reduce 加 broadcast 的效果，只是 HPL 没有直接调用标准集合通信，而是自己用成对消息实现。

#### 7. `HPL_bcast` 与 `HPL_broadcast` 有什么本质区别？

答：

- `HPL_bcast`
  面向面板广播，是 HPL 面板通信路径的一部分，使用可选虚拟拓扑，如 1-ring、2-ring、long 等。
- `HPL_broadcast`
  是 HPL 的通用广播基础设施，语义上是 broadcast，但实现上通常是 HPL 自己写的点对点树形传播。

前者更贴近算法级面板转发，后者更贴近库级集合基础设施。

#### 8. `HPL_reduce`、`HPL_all_reduce` 用到了什么算子？

答：

常见算子有：

- `HPL_max`
- `HPL_min`
- `HPL_sum`

其中：

- `max` 常用于最大范数、错误状态传播、最大耗时统计。
- `sum` 常用于残差或局部贡献累加。
- `min` 常用于一些辅助统计与计时分析。

#### 9. `HPL_pdlaswp00N`、`HPL_rollN`、`HPL_spreadN` 为什么不用标准 MPI collective 直接替代？

答：

因为这里的数据归属关系高度依赖主元模式：

- 谁需要哪些行不是固定的；
- 消息大小在不同进程上可能不同；
- 部分步骤还希望和面板广播进度重叠。

这种场景下，HPL 更倾向于自定义点对点调度，而不是使用抽象层更高但灵活性更低的标准集合通信。

#### 10. `HPL_pdtrsv` 为什么说是低时延流水线？

答：

因为回代阶段是块级依赖推进的：

- 当前块求出来之后，下一块才能继续；
- 解向量块需要在行、列通信器之间逐步传播；
- 每一步解锁下一步。

因此这个阶段的性能更受消息时延影响，而不是单纯受大带宽影响。

### 四、理解题

#### 11. 为什么 HPL 中很多“集合语义操作”仍然是点对点实现？

答：

因为 HPL 更看重：

- 路径可控
- 重叠可控
- 消息编号可控
- 数据布局可控
- 面向特定算法关键路径的调优能力

这也是超算数值库里非常典型的设计思路：逻辑上看像 collective，工程实现上却往往是 carefully scheduled point-to-point。

#### 12. 为什么阅读这些网页时必须结合源码？

答：

因为网页文档更多给你的是：

- 接口说明
- 术语定义
- 算法直观解释

而真正决定行为的是源码中的：

- 调度顺序
- 通信时机
- 条件分支
- 具体的数据布局和消息路径

尤其在 HPL 里，源码比网页文档更接近真实实现。

## 6. 最后的建议

如果你的目标是做汇报、答辩、考试或深入分析，建议固定采用下面这条最短闭环阅读路径：

1. `www/index.html`
2. `www/algorithm.html`
3. `www/tuning.html`
4. `www/HPL_pddriver.html`
5. `www/HPL_pdtest.html`
6. `www/HPL_pdgesv.html`
7. `www/HPL_pdgesv0.html`
8. `www/HPL_pdgesvK2.html`
9. `www/HPL_pdfact.html`
10. `www/HPL_pdmxswp.html`
11. `www/HPL_bcast.html`
12. `www/HPL_broadcast.html`
13. `www/HPL_reduce.html`
14. `www/HPL_pdupdateNN.html`
15. `www/HPL_pdlaswp00N.html`
16. `www/HPL_rollN.html`
17. `www/HPL_spreadN.html`
18. `www/HPL_pdtrsv.html`

如果按这条路径读完，再结合源码中的相对链接跳转，你基本就能把 HPL 的算法流程和通信骨架完整地串起来。
