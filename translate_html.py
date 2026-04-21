#!/usr/bin/env python3
"""
Batch translate HPL HTML documentation files with bilingual (EN/CN) layout.
Adds Chinese translations after each English text block.
"""

import os
import re

NAV_BAR = """<CENTER>
<A HREF = "index.html">            [Home / 首页]</A>
<A HREF = "copyright.html">        [Copyright and Licensing Terms / 版权和许可条款]</A>
<A HREF = "algorithm.html">        [Algorithm / 算法]</A>
<A HREF = "scalability.html">      [Scalability / 可扩展性]</A>
<A HREF = "results.html">          [Performance Results / 性能结果]</A>
<A HREF = "documentation.html">    [Documentation / 文档]</A>
<A HREF = "software.html">         [Software / 软件]</A>
<A HREF = "faqs.html">             [FAQs / 常见问题]</A>
<A HREF = "tuning.html">           [Tuning / 调优]</A>
<A HREF = "errata.html">           [Errata-Bugs / 勘误-错误]</A>
<A HREF = "references.html">       [References / 参考文献]</A>
<A HREF = "links.html">            [Related Links / 相关链接]</A><BR>
</CENTER>"""

translations = {
    "scalability.html": {
        "title": "HPL Scalability Analysis / HPL 可扩展性分析",
        "h2": "HPL Scalability Analysis / HPL 可扩展性分析",
        "intro": (
            'The <A HREF = "scalability.html#model">machine model</A> used for the '
            'analysis is first described.  This crude model is then used  to first '
            'estimate  the  parallel running time  of  the various phases  of  the '
            'algorithm namely\n'
            '<UL>\n'
            '<LI><A HREF="scalability.html#pfact">panel factorization and broadcast</A>,\n'
            '<LI><A HREF="scalability.html#updat">trailing submatrix update</A>, \n'
            '<LI><A HREF="scalability.html#backs">backward substitution</A>. \n'
            '</UL>\n'
            'Finally <A HREF="scalability.html#total">the  parallel efficiency</A> '
            'of the entire algorithm is estimated according to this machine model. '
            'We show that for a given set of parameters HPL is <STRONG>scalable</STRONG> '
            'not  only  with respect to the amount of computation,  but  also with '
            'respect to the communication volume.'
        ),
        "intro_cn": (
            '首先描述用于分析的<A HREF = "scalability.html#model">机器模型</A>。'
            '然后使用这个粗略模型来估计算法各阶段的并行运行时间，即\n'
            '<UL>\n'
            '<LI><A HREF="scalability.html#pfact">面板分解和广播</A>，\n'
            '<LI><A HREF="scalability.html#updat">尾部子矩阵更新</A>，\n'
            '<LI><A HREF="scalability.html#backs">回代</A>。\n'
            '</UL>\n'
            '最后根据此机器模型估计整个算法的<A HREF="scalability.html#total">并行效率</A>。'
            '我们表明，对于给定的参数集，HPL 不仅在计算量方面是<strong>可扩展的</strong>，'
            '而且在通信量方面也是可扩展的。'
        ),
        "model_h3": "The Machine Model / 机器模型",
        "model_p1": (
            "Distributed-memory computers consist of processors that are connected "
            "using  a message passing interconnection network.  Each processor has "
            "its own memory called the local memory,  which  is accessible only to "
            "that processor.  As the time to access a remote memory is longer than "
            "the time to access a local one,  such computers are often referred to "
            "as Non-Uniform Memory Access (NUMA) machines."
        ),
        "model_p1_cn": (
            "分布式内存计算机由通过消息传递互连网络连接的处理器组成。每个处理器都有自己的内存，"
            "称为本地内存，只能被该处理器访问。由于访问远程内存的时间比访问本地内存的时间长，"
            "这种计算机通常被称为非一致性内存访问（NUMA）机器。"
        ),
        "model_p2": (
            "The interconnection network  of our machine model is static,  meaning "
            "that   it   consists  of  point-to-point  communication  links  among "
            "processors.  This  type  of  network  is also referred to as a direct "
            "network as opposed to dynamic networks.  The  latter  are constructed  "
            "from switches and communication links.  These links  are  dynamically "
            "connected  to one another by the switching elements to establish,  at "
            "run time, the paths between processors memories."
        ),
        "model_p2_cn": (
            "我们机器模型的互连网络是静态的，即它由处理器之间的点对点通信链路组成。"
            "这种类型的网络也称为直接网络，与动态网络相对。后者由交换机和通信链路构成。"
            "这些链路由交换元件动态地相互连接，以在运行时建立处理器内存之间的路径。"
        ),
        "model_p3": (
            "The  interconnection  network  of the two-dimensional  machine  model "
            "considered here is a static,  fully  connected physical topology.  It "
            "is also assumed  that  processors  can be treated  equally  in  terms "
            "of  local performance  and  that  the  communication rate between two "
            "processors depends on the processors considered."
        ),
        "model_p3_cn": (
            "这里考虑的二维机器模型的互连网络是静态的、全连接的物理拓扑。"
            "还假设处理器在本地性能方面可以被同等对待，两个处理器之间的通信速率取决于所考虑的处理器。"
        ),
        "model_p4": (
            "Our model assumes  that  a processor can send or receive data on only "
            "one of its communication ports at a time  (assuming  it has more than "
            "one). In the literature,  this  assumption is also referred to as the "
            "one-port communication model."
        ),
        "model_p4_cn": (
            "我们的模型假设处理器一次只能在其一个通信端口上发送或接收数据"
            "（假设它有多个端口）。在文献中，这种假设也被称为单端口通信模型。"
        ),
        "model_p5": (
            "The time spent to communicate  a message between two given processors "
            "is called the communication time Tc.   In  our machine model,  Tc  is "
            "approximated  by  a  linear  function  of  the number  L  of  double "
            "precision (64-bits) items communicated.  Tc is the sum of the time to "
            "prepare the message for transmission (alpha) and the time  (beta * L) "
            "taken  by the message of length  L  to traverse  the network  to  its  "
            "destination, i.e.,"
        ),
        "model_p5_cn": (
            "在两个给定处理器之间通信一条消息所花费的时间称为通信时间 Tc。"
            "在我们的机器模型中，Tc 由通信的双精度（64位）项数 L 的线性函数近似。"
            "Tc 是准备消息传输的时间（alpha）和长度为 L 的消息穿越网络到达目的地的时间（beta * L）之和，即："
        ),
        "model_p6": (
            "Finally,   the   model  assumes  that  the  communication  links  are "
            "bi-directional,  that is,  the time  for two processors  to send each  "
            "other a message of length L is also Tc.  A processor  can send and/or "
            "receive  a message on only one of  its communication links at a time. "
            "In particular, a processor can send a message while receiving another "
            "message from the processor it is sending to at the same time."
        ),
        "model_p6_cn": (
            "最后，模型假设通信链路是双向的，即两个处理器互相发送长度为 L 的消息的时间也是 Tc。"
            "处理器一次只能在其一个通信链路上发送和/或接收消息。"
            "特别是，处理器可以在向某个处理器发送消息的同时从该处理器接收另一条消息。"
        ),
        "model_p7": (
            "Since this document is only concerned with regular local dense linear "
            "algebra  operations,  the time taken to perform  one  floating  point  "
            "operation  is  assumed  to  be  summarized by  three constants  gam1,  "
            "gam2 and gam3. These quantitites are flop rates approximations of the "
            "vector-vector,  matrix-vector  and matrix-matrix operations for  each "
            "processor.  This  very  crude approximation summarizes all  the steps "
            "performed  by a processor  to achieve such a computation.  Obviously, "
            "such a model neglects all the phenomena  occurring  in  the processor "
            "components,  such as cache misses, pipeline startups, memory load  or "
            "store, floating point arithmetic and so on,  that  may  influence the "
            "value  of  these  constants  as a function  of the  problem size for "
            "example."
        ),
        "model_p7_cn": (
            "由于本文档仅涉及常规的局部稠密线性代数操作，执行一次浮点操作所花费的时间"
            "假设由三个常数 gam1、gam2 和 gam3 概括。这些量是每个处理器的向量-向量、"
            "矩阵-向量和矩阵-矩阵操作的浮点速率近似。这种非常粗略的近似概括了处理器"
            "执行此类计算所执行的所有步骤。显然，这种模型忽略了处理器组件中发生的所有现象，"
            "如缓存未命中、流水线启动、内存加载或存储、浮点算术等，这些现象可能影响"
            "这些常数的值作为问题规模的函数。"
        ),
        "model_p8": (
            "Similarly,  the model  does  not make any assumption on the amount of "
            "physical memory per node.  It  is  assumed that if a process has been "
            "spawn  on  a processor,  one  has  ensured  that  enough  memory  was  "
            "available  on that processor. In other words, swapping will not occur "
            "during the modeled computation."
        ),
        "model_p8_cn": (
            "类似地，模型不对每个节点的物理内存量做任何假设。假设如果在处理器上生成了一个进程，"
            "则已确保该处理器上有足够的可用内存。换句话说，在建模的计算过程中不会发生交换。"
        ),
        "model_note": (
            "This  machine  model  is  a very crude approximation that is designed "
            "specifically  to  illustrate  the cost of the dominant factors of our "
            "particular case."
        ),
        "model_note_cn": (
            "这个机器模型是一个非常粗略的近似，专门设计用于说明我们特定情况的主要因素的成本。"
        ),
        "pfact_h3": "Panel Factorization and Broadcast / 面板分解和广播",
        "pfact_p": (
            "Let  consider  an  M-by-N  panel distributed over a P-process column. "
            "Because  of the recursive formulation of the panel factorization,  it "
            "is  reasonable to consider  that  the floating point operations  will "
            "be performed at matrix-matrix multiply \"speed\".  For  every column in "
            "the panel a binary-exchange is performed on 2*N data items. When this "
            "panel is broadcast,  what  matters  is the time that the next process "
            "column  will  spend  in this  communication operation.  Assuming  one "
            "chooses the <A HREF=\"algorithm.html#bcast\">increasing-ring (modified) "
            "variant</A>,  only  one  message needs to be taken into account.  The "
            "execution  time  of the panel factorization and broadcast can thus be "
            "approximated by:"
        ),
        "pfact_p_cn": (
            "考虑分布在 P 个进程列上的 M×N 面板。由于面板分解的递归公式，"
            "可以合理地认为浮点操作将以矩阵-矩阵乘法的\u201c速度\u201d执行。"
            "对于面板中的每一列，对 2*N 个数据项执行二进制交换。"
            "当此面板被广播时，重要的是下一个进程列在此通信操作中花费的时间。"
            "假设选择<A HREF=\"algorithm.html#bcast\">递增环（修改版）变体</A>，"
            "只需要考虑一条消息。因此，面板分解和广播的执行时间可以近似为："
        ),
        "updat_h3": "Trailing Submatrix Update / 尾部子矩阵更新",
        "updat_p": (
            "Let  consider  the  update  phase  of an  N-by-N  trailing  submatrix "
            "distributed on a P-by-Q process grid.  From  a computational point of "
            "view one has to (triangular) solve N right-hand-sides  and  perform a  "
            "local rank-NB update of this trailing submatrix. Assuming one chooses "
            "the <A HREF=\"algorithm.html#update\">long variant</A>,  the  execution "
            "time of the update operation can be approximated by:"
        ),
        "updat_p_cn": (
            "考虑分布在 P×Q 进程网格上的 N×N 尾部子矩阵的更新阶段。"
            "从计算的角度来看，需要（三角）求解 N 个右端项，并执行此尾部子矩阵的局部秩-NB 更新。"
            "假设选择<A HREF=\"algorithm.html#update\">长条变体</A>，"
            "更新操作的执行时间可以近似为："
        ),
        "updat_note": (
            'The constant "3" in front of the "beta" term is obtained  by counting '
            "one for the (logarithmic) spread phase and two for the rolling phase; "
            "In the case of bi-directional links  this constant 3 should therefore "
            "be only a 2."
        ),
        "updat_note_cn": (
            '"beta" 项前面的常数 "3" 是通过计算（对数）散射阶段为一个和滚动阶段为两个得到的；'
            "在双向链路的情况下，此常数 3 应该仅为 2。"
        ),
        "backs_h3": "Backward Substitution / 回代",
        "backs_p": (
            "The number of floating point operations performed during the backward "
            "substitution in given by  N^2 / (P*Q).  Because of the lookahead, the "
            "communication cost  can be approximated at each step  by two messages "
            "of length NB, i.e.,  the time  to  communicate  the NB-piece  of  the  "
            "solution vector from one diagonal block of the matrix to another.  It "
            "follows that the execution time of the backward substitution  can  be "
            "approximated by:"
        ),
        "backs_p_cn": (
            "回代期间执行的浮点操作数为 N^2 / (P*Q)。由于前瞻，"
            "每一步的通信成本可以近似为两条长度为 NB 的消息，"
            "即从矩阵的一个对角块到另一个对角块通信解向量的 NB 片段的时间。"
            "因此，回代的执行时间可以近似为："
        ),
        "total_h3": "Putting it All Together / 综合分析",
        "total_p1": (
            "The total execution time of the algorithm described above is given by"
        ),
        "total_p1_cn": "上述算法的总执行时间由下式给出",
        "total_p2": (
            "That is, by only considering only the dominant term in alpha, beta and "
            "gam3:"
        ),
        "total_p2_cn": "即，仅考虑 alpha、beta 和 gam3 中的主导项：",
        "total_p3": (
            "The serial execution time is given by Tser = 2 gam3 N^3  / 3. If we "
            "define the parallel efficiency  E  as the ratio  Tser / ( P Q Thpl ), we "
            "obtain:"
        ),
        "total_p3_cn": (
            "串行执行时间由 Tser = 2 gam3 N^3 / 3 给出。如果我们将并行效率 E 定义为"
            "比率 Tser / (P Q Thpl)，我们得到："
        ),
        "total_p4": (
            "This  last equality  shows  that when the memory usage per  processor "
            "N^2 / (P Q)  is maintained  constant, the parallel efficiency  slowly "
            "decreases  only  because of the alpha term.  The communication volume "
            "(the beta term) however remains constant.  Due to these results,  HPL "
            "is said to be <STRONG>scalable</STRONG> not only with respect  to the "
            "amount of computation,  but also  with  respect  to the communication "
            "volume."
        ),
        "total_p4_cn": (
            "最后一个等式表明，当每个处理器的内存使用量 N^2 / (P Q) 保持恒定时，"
            "并行效率仅因为 alpha 项而缓慢下降。然而，通信量（beta 项）保持恒定。"
            "由于这些结果，HPL 被称为<strong>可扩展的</strong>，"
            "不仅在计算量方面，而且在通信量方面也是可扩展的。"
        ),
    },
    "faqs.html": {
        "title": "HPL Frequently Asked Questions / HPL 常见问题",
        "h2": "HPL Frequently Asked Questions / HPL 常见问题",
        "q1": "What problem size N should I run ? / 我应该运行多大的问题规模 N ？",
        "a1": (
            "In order  to find out  the  best performance   of  your  system,  the "
            "largest   problem size  fitting in memory is what you should aim for. "
            "The  amount  of  memory  used  by  HPL is essentially the size of the  "
            "coefficient matrix.  So for example, if you have 4 nodes  with 256 Mb "
            "of memory on each, this corresponds to 1 Gb total, i.e., 125 M double "
            "precision  (8  bytes)  elements. The  square  root  of that number is "
            "11585.  One  definitely needs to leave some memory for the OS as well "
            "as for other things, so a problem size of 10000 is likely to fit.  As "
            "a rule of thumb, 80 % of the  total amount of memory is a good guess. "
            "If the problem size you pick is too large,  swapping will occur,  and "
            "the performance will drop.  If multiple processes  are spawn  on each "
            "node  (say  you have 2 processors  per  node),  what  counts  is  the "
            "available amount of memory to each process."
        ),
        "a1_cn": (
            "为了找出系统的最佳性能，您应该以适合内存的最大问题规模为目标。"
            "HPL 使用的内存量基本上是系数矩阵的大小。例如，如果您有 4 个节点，每个有 256 MB 内存，"
            "这对应于总共 1 GB，即 125M 个双精度（8 字节）元素。该数字的平方根为 11585。"
            "肯定需要为操作系统和其他东西留出一些内存，因此问题规模 10000 可能适合。"
            "作为经验法则，总内存量的 80% 是一个好的猜测。如果您选择的问题规模太大，将发生交换，"
            "性能将下降。如果每个节点上生成了多个进程（例如每个节点有 2 个处理器），"
            "重要的是每个进程可用的内存量。"
        ),
        "q2": "What block size NB should I use ? / 我应该使用什么块大小 NB ？",
        "a2": (
            "HPL  uses  the block size NB for the data distribution as well as for "
            "the  computational  granularity.  From  a data distribution  point of "
            "view,  the smallest NB,  the better the load balance.  You definitely "
            "want  to stay away  from very large values of NB.  From a computation "
            "point of view,  a too small value of NB  may  limit the computational "
            "performance by a large factor because almost no data reuse will occur "
            "in the highest level of the memory hierarchy. The  number of messages "
            "will  also  increase.  Efficient  matrix-multiply  routines are often  "
            "internally  blocked.  Small  multiples  of  this  blocking factor are  "
            "likely to be good block sizes for HPL. The bottom line is that \"good\" "
            "block sizes are almost always in the [32 .. 256] interval.  The  best "
            "values depend on the computation / communication performance ratio of "
            "your system. To a much less extent, the problem size matters as well. "
            "Say for example,  you emperically found that 44 was a good block size "
            "with respect to performance.  88 or 132  are likely  to give slightly  "
            "better results  for large problem sizes because of a slighlty  higher "
            "flop rate."
        ),
        "a2_cn": (
            "HPL 使用块大小 NB 进行数据分布和计算粒度。从数据分布的角度来看，"
            "NB 越小，负载平衡越好。您一定要避免使用非常大的 NB 值。"
            "从计算的角度来看，NB 值太小可能会大幅限制计算性能，"
            "因为在内存层次结构的最高层几乎不会发生数据重用。消息数量也会增加。"
            "高效的矩阵乘法例程通常在内部是分块的。此分块因子的小倍数可能是 HPL 的良好块大小。"
            "底线是「好」的块大小几乎总是在 [32 .. 256] 区间内。最佳值取决于系统的计算/通信性能比。"
            "问题规模的影响要小得多。例如，如果您凭经验发现 44 是一个性能良好的块大小，"
            "那么 88 或 132 可能会在大问题规模上给出略好的结果，因为浮点速率略高。"
        ),
        "q3": "What process grid ratio P x Q should I use ? / 我应该使用什么进程网格比例 P × Q ？",
        "a3": (
            "This  depends  on  the  physical  interconnection  network  you have. "
            "Assuming a mesh or a switch HPL \"likes\" a 1:k ratio with k in [1..3]. "
            "In  other  words,  P  and  Q  should  be approximately equal,  with Q  "
            "slightly larger than P. Examples: 2 x 2, 2 x 4, 2 x 5,  3 x 4, 4 x 4, "
            "4 x 6, 5 x 6, 4 x 8 ...  If  you  are  running  on  a simple Ethernet  "
            "network,  there  is only one wire through which all the messages are "
            "exchanged. On  such a network, the performance and scalability of HPL "
            "is strongly limited  and very flat process grids are likely to be the "
            "best choices: 1 x 4, 1 x 8, 2 x 4 ..."
        ),
        "a3_cn": (
            "这取决于您的物理互连网络。假设是网格或交换机，HPL 「喜欢」 1:k 的比例，"
            "其中 k 在 [1..3] 范围内。换句话说，P 和 Q 应该大致相等，Q 略大于 P。"
            "例如：2×2, 2×4, 2×5, 3×4, 4×4, 4×6, 5×6, 4×8 ..."
            "如果您在简单的以太网上运行，只有一条线路用于交换所有消息。"
            "在这样的网络上，HPL 的性能和可扩展性受到严重限制，"
            "非常扁平的进程网格可能是最佳选择：1×4, 1×8, 2×4 ..."
        ),
        "q4": "What about the one processor case ? / 单处理器情况如何 ？",
        "a4": (
            "HPL  has  been  designed  to  perform well for large problem sizes on "
            "hundreds  of  nodes and more.  The software works on one node and for "
            "large problem sizes, one  can usually achieve pretty good performance "
            "on a single processor as well.  For small problem sizes  however, the "
            "overhead  due  to  message-passing,  local  indexing and so on can be  "
            "significant."
        ),
        "a4_cn": (
            "HPL 的设计目标是在数百个节点或更多节点上对大问题规模表现良好。"
            "该软件可以在一个节点上运行，对于大问题规模，通常也可以在单个处理器上获得相当好的性能。"
            "然而，对于小问题规模，消息传递、本地索引等开销可能很显著。"
        ),
        "q5": "Why so many options in HPL.dat ? / 为什么 HPL.dat 中有这么多选项 ？",
        "a5": (
            "There are quite a few reasons. First off, these options are useful to "
            "determine what matters and what does not on your system. Second,  HPL "
            "is often used in the context  of early evaluation of new systems.  In "
            "such a case, everything is usually not quite working right, and it is "
            "convenient  to be able  to vary these parameters without recompiling. "
            "Finally,  every system has its own peculiarities and one is likely to "
            "be  willing  to  emperically determine the best set of parameters. In "
            "any   case,  one  can  always  follow  the  advice  provided  in  the "
            '<A HREF = "tuning.html">tuning  section</A> of this  document and not '
            "worry about the complexity of the input file."
        ),
        "a5_cn": (
            "有很多原因。首先，这些选项有助于确定在您的系统上什么重要什么不重要。"
            "其次，HPL 经常在新系统的早期评估中使用。在这种情况下，通常一切都不太正常，"
            "能够不重新编译就改变这些参数是很方便的。最后，每个系统都有自己的特点，"
            "用户可能愿意通过实验确定最佳的参数集。无论如何，您可以始终遵循本文档"
            '<A HREF = "tuning.html">调优部分</A>中提供的建议，而不必担心输入文件的复杂性。'
        ),
        "q6": "Can HPL be Outperformed ? / HPL 可以被超越吗 ？",
        "a6": (
            "Certainly.   There  is  always  room  for  performance  improvements. "
            "Specific knowledge about  a  particular system  is always a source of "
            "performance   gains.  Even  from  a generic  point  of  view,  better "
            "algorithms  or  more  efficient  formulation  of the classic ones are "
            "potential winners."
        ),
        "a6_cn": (
            "当然。性能改进总是有空间的。对特定系统的专门知识总是性能提升的来源。"
            "即使从通用角度来看，更好的算法或经典算法的更高效公式化也可能是赢家。"
        ),
    },
    "software.html": {
        "title": "HPL Software / HPL 软件",
        "h2": "HPL Software / HPL 软件",
        "h3_download": "Download and Installation / 下载和安装",
        "step1": (
            'Download    the  <A HREF="hpl-2.3.tar.gz">tar-gzipped  file</A>, '
            'issue  then "gunzip hpl-2.3.tar.gz; tar -xvf hpl-2.3.tar"  and  this '
            "should create an  hpl-2.3  directory  containing  the  distribution. "
            "We call this directory the top level directory."
        ),
        "step1_cn": (
            '下载<A HREF="hpl-2.3.tar.gz">tar-gzipped 文件</A>，'
            '然后执行 "gunzip hpl-2.3.tar.gz; tar -xvf hpl-2.3.tar"，'
            '这将创建一个包含发行版的 hpl-2.3 目录。我们称此目录为顶层目录。'
        ),
        "step2": (
            "Create a  file  Make.&#60arch&#62  in  the  top-level directory. "
            "For  this purpose,  you  may  want  to  re-use  one contained in the  "
            "setup  directory.  This Make.&#60arch&#62 file  essentially contains "
            "the compilers, libraries, and their paths to be used on your system."
        ),
        "step2_cn": (
            "在顶层目录中创建文件 Make.&#60arch&#62。为此，您可能想要重用 setup 目录中"
            "包含的文件。此 Make.&#60arch&#62 文件基本上包含要在您的系统上使用的编译器、"
            "库及其路径。"
        ),
        "step3": (
            'Type  "make arch=&#60arch&#62". This should create an executable '
            "in the bin/&#60arch&#62 directory called xhpl.  For example,  on our "
            "Linux  PII  cluster,  I create  a file called Make.Linux_PII in  the "
            'top-level  directory.  Then,  I  type  "make  arch=Linux_PII".  This '
            "creates  the executable file bin/Linux_PII/xhpl."
        ),
        "step3_cn": (
            '输入 "make arch=&#60arch&#62"。这应该在 bin/&#60arch&#62 目录中创建'
            '一个名为 xhpl 的可执行文件。例如，在我们的 Linux PII 集群上，'
            '我在顶层目录中创建了一个名为 Make.Linux_PII 的文件。然后，我输入 '
            '"make arch=Linux_PII"。这将创建可执行文件 bin/Linux_PII/xhpl。'
        ),
        "step4": (
            "Quick check:  run  a few  tests  (assuming  you have 4 nodes for "
            "interactive use)  by  issuing  the  following  commands from the top "
            'level  directory:  "cd bin/&#60arch&#62 ;  mpirun -np 4 xhpl".  This '
            "should produce quite a bit of meaningful output on the screen."
        ),
        "step4_cn": (
            "快速检查：通过从顶层目录发出以下命令来运行一些测试"
            '（假设您有 4 个节点可用于交互使用）："cd bin/&#60arch&#62 ; mpirun -np 4 xhpl"。'
            "这应该在屏幕上产生相当多有意义的输出。"
        ),
        "step5": (
            "Most  of  the  performance parameters can be tuned, by modifying "
            "the input file bin/&#60arch&#62/HPL.dat. See the "
            '<A HREF = "tuning.html">tuning page</A>  or  the  TUNING file in the '
            "top-level directory."
        ),
        "step5_cn": (
            "大多数性能参数可以通过修改输入文件 bin/&#60arch&#62/HPL.dat 来调整。"
            '参见<A HREF = "tuning.html">调优页面</A>或顶层目录中的 TUNING 文件。'
        ),
        "h3_compile": "Compile Time Options / 编译时选项",
        "compile_intro": (
            "At  the  end  of  the \"model\" Make.&#60arch&#62,  the  user is given "
            "the  opportunity  to override  some default  compile options of this "
            "software. The list of these options and their meaning is:"
        ),
        "compile_intro_cn": (
            "在「模型」 Make.&#60arch&#62 的末尾，用户有机会覆盖此软件的一些默认编译选项。"
            "这些选项及其含义的列表如下："
        ),
        "opt1": "force the copy of the panel L before bcast / 强制在广播前复制面板 L",
        "opt2": "call the BLAS C interface / 调用 BLAS C 接口",
        "opt3": "call the vsip library / 调用 vsip 库",
        "opt4": "enable detailed timers / 启用详细计时器",
        "compile_note": (
            "The user must choose between either the  BLAS  Fortran 77 interface, "
            "or the BLAS C interface, or the  VSIPL  library depending  on  which "
            "computational kernels are available on his system. Only one of these "
            "options should be selected.  If  you  choose  the  BLAS  Fortran  77 "
            "interface,  it is necessary  to fill out  the machine-specific  C to "
            "Fortran 77 interface section of the  Make.&#60arch&#62  file.  To do "
            "this,  please  refer  to the Make.&#60arch&#62 examples contained in "
            "the setup directory."
        ),
        "compile_note_cn": (
            "用户必须根据其系统上可用的计算内核在 BLAS Fortran 77 接口、BLAS C 接口或 VSIPL 库"
            "之间进行选择。只应选择这些选项中的一个。如果您选择 BLAS Fortran 77 接口，"
            "需要填写 Make.&#60arch&#62 文件中特定于机器的 C 到 Fortran 77 接口部分。"
            "为此，请参考 setup 目录中包含的 Make.&#60arch&#62 示例。"
        ),
        "default_note": "By default HPL will: / 默认情况下 HPL 将：",
        "default1": "not copy L before broadcast, / 广播前不复制 L，",
        "default2": "call the BLAS Fortran 77 interface, / 调用 BLAS Fortran 77 接口，",
        "default3": "not display detailed timing information. / 不显示详细的计时信息。",
        "example_note": (
            "As an example,  suppose one wants this software to copy the panel of "
            "columns  into  a contiguous buffer  before broadcasting.  It  should "
            "be  more efficient  to let  the software create the appropriate  MPI "
            "user-defined data type  since this may avoid the data copy.  So,  it "
            "is a strange idea,  but one insists.  To achieve this  one would add "
            "-DHPL_COPY_L  to  the definition of  HPL_OPTS at the end of the file "
            "Make.&#60arch&#62.  Issue   then  a  \"make clean arch=&#60arch&#62 ;  "
            "make build arch=&#60arch&#62\"  and  the executable  will be re-build "
            "with that feature in."
        ),
        "example_note_cn": (
            "例如，假设用户希望此软件在广播之前将列面板复制到连续缓冲区中。"
            "让软件创建适当的 MPI 用户定义数据类型应该更高效，因为这可以避免数据复制。"
            "所以，这是一个奇怪的想法，但用户坚持。要实现这一点，"
            "可以在 Make.&#60arch&#62 文件末尾的 HPL_OPTS 定义中添加 -DHPL_COPY_L。"
            "然后执行 \"make clean arch=&#60arch&#62 ; make build arch=&#60arch&#62\"，"
            "可执行文件将重新构建并包含该功能。"
        ),
    },
    "copyright.html": {
        "title": "HPL Copyright and Licensing Terms / HPL 版权和许可条款",
        "h2": "HPL Copyright Notice and Licensing Terms / HPL 版权声明和许可条款",
        "license_intro": (
            "Redistribution  and  use in  source and binary forms, with or without "
            "modification, are  permitted provided  that the following  conditions "
            "are met:"
        ),
        "license_intro_cn": "在满足以下条件的前提下，允许以源代码和二进制形式重新分发和使用，无论是否经过修改：",
        "lic1": "Redistributions  of  source code  must retain the above copyright notice, this list of conditions and the following disclaimer. / 源代码的重新分发必须保留上述版权声明、此条件列表和以下免责声明。",
        "lic2": "Redistributions in binary form must reproduce the above copyright notice, this list of conditions,  and the following disclaimer in the documentation and/or other materials provided with the distribution. / 二进制形式的重新分发必须在随分发提供的文档和/或其他材料中复制上述版权声明、此条件列表和以下免责声明。",
        "lic3": "All  advertising  materials mentioning  features  or  use of this software must display  the  following  acknowledgement:  This product includes   software   developed   at  the  University  of  Tennessee, Knoxville, Innovative Computing Laboratory. / 所有提及本软件特性或使用的广告材料必须显示以下致谢：本产品包含在田纳西大学诺克斯维尔分校创新计算实验室开发的软件。",
        "lic4": "The name of the  University, the name of the  Laboratory,  or the names  of  its  contributors  may  not  be used to endorse or promote products  derived   from   this  software  without  specific  written permission. / 未经特定书面许可，大学、实验室的名称或其贡献者的名称不得用于认可或推广源自本软件的产品。",
        "disclaimer_h3": "Disclaimer / 免责声明",
        "disclaimer_cn": (
            "本软件由版权持有者和贡献者「按原样「提供。任何明示或暗示的保证，包括但不限于"
            "适销性和特定用途适用性的暗示保证，均被拒绝。在任何情况下，大学或贡献者都不对任何"
            "直接、间接、附带、特殊、惩罚性或后果性损害（包括但不限于替代商品或服务的采购、"
            "使用、数据或利润的损失或业务中断）负责，无论此类损害是如何引起的，也无论基于何种"
            "责任理论，无论是合同责任、严格责任还是侵权（包括疏忽或其他），即使已被告知可能发生"
            "此类损害，本软件的使用也不承担任何责任。"
        ),
    },
    "links.html": {
        "title": "HPL Related Links / HPL 相关链接",
        "h2": "HPL Related Links / HPL 相关链接",
        "intro": (
            "The  list  of links below contains some relevant material to this "
            "work.  This  list  is provided  for illustrative purposes, and  should be "
            "regarded  as  an initial starting point  for the interested reader.  This "
            "list is by all means not meant to be exhaustive."
        ),
        "intro_cn": (
            "下面的链接列表包含与此工作相关的一些材料。此列表仅供说明目的，"
            "应被视为有兴趣的读者的初始起点。此列表绝不是详尽无遗的。"
        ),
        "mpi_h3": "Message Passing Interface (MPI) / 消息传递接口 (MPI)",
        "mpi_desc": (
            "MPI  is  a  library  specification  for  message-passing,  proposed  as a "
            "standard  by  a  broadly  based committee  of  vendors, implementors, and "
            "users.  Machine-specific (optimized)  as  well  as  freely available  MPI "
            "libraries  are  available  for  a large  variety of systems.  Browse  the "
            '<A HREF = "http://www.mcs.anl.gov/mpi">Message  Passing  Interface  (MPI) '
            "standard web page</A> for more information."
        ),
        "mpi_desc_cn": (
            "MPI 是一个消息传递的库规范，由供应商、实现者和用户组成的广泛委员会提议为标准。"
            "机器特定（优化）的和免费可用的 MPI 库可用于多种系统。"
            '浏览<A HREF = "http://www.mcs.anl.gov/mpi">消息传递接口 (MPI) 标准网页</A>获取更多信息。'
        ),
        "blas_h3": "Basic Linear Algebra Subroutines (BLAS) / 基本线性代数子程序 (BLAS)",
        "blas_desc": (
            'The  <A HREF = "http://www.netlib.org/blas">BLAS</A>  are   high  quality '
            '"building  block"   routines  for  performing  basic vector  and  matrix '
            "operations.  A  lot  of  \"BLAS-related\"  information can be found at this "
            "site.  In  particular,  a  reference implementation  is  available.  This "
            "reference   implementation  is  <STRONG>not  optimized</STRONG>  for  any "
            "system, and  it is therefore <STRONG>not  recommended</STRONG>  to use it "
            'for  benchmarking  purposes</STRONG>. '
            'However, <A HREF = "http://www.netlib.org/blas/faq.html">machine-specific '
            "optimized  BLAS  libraries</A> are  available  for  a variety of computer "
            "systems.   For  further  details,    please  contact  your  local  vendor "
            "representative.  Alternatively,  one  may  also consider  using automatic "
            'code  generators such as <A HREF="http://www.netlib.org/atlas">ATLAS</A>. '
            "This  tool  automatically   generates   a  complete   and  optimized BLAS "
            "library for a large variety of modern systems."
        ),
        "blas_desc_cn": (
            '<A HREF = "http://www.netlib.org/blas">BLAS</A> 是用于执行基本向量和矩阵操作的'
            '高质量「构建块「例程。在此站点上可以找到大量"BLAS 相关「信息。'
            '特别是，提供了参考实现。此参考实现<strong>未针对任何系统进行优化</strong>，'
            '因此<strong>不建议</strong>将其用于基准测试目的。'
            '但是，<A HREF = "http://www.netlib.org/blas/faq.html">机器特定优化的 BLAS 库</A>'
            '可用于多种计算机系统。有关更多详情，请联系您当地的供应商代表。'
            '或者，也可以考虑使用自动代码生成器，如<A HREF="http://www.netlib.org/atlas">ATLAS</A>。'
            '此工具自动为多种现代系统生成完整且优化的 BLAS 库。'
        ),
        "vsip_h3": "Vector Signal Image Processing Library (VSIPL) / 向量信号图像处理库 (VSIPL)",
        "vsip_desc": (
            '<A HREF="https://www.omg.org/spec/VSIPL/">VSIPL</A> is an API defined  by '
            "an  open  standard  comprised  of  embedded  signal and  image processing "
            "hardware and software vendors,  academia,  users, and government labs.  A "
            "lot of  \"VSIPL-related\"  information can be  found  at  this  site.  In "
            "particular, a  reference  implementation is available.  Machine-specific "
            "optimized  VSIPL  libraries  are  available  for  a variety  of computer "
            "systems.   For  further  details,   please contact   your   local  vendor "
            "representative."
        ),
        "vsip_desc_cn": (
            '<A HREF="https://www.omg.org/spec/VSIPL/">VSIPL</A> 是由嵌入式信号和图像处理'
            '硬件和软件供应商、学术界、用户和政府实验室组成的开放标准定义的 API。'
            '在此站点上可以找到大量"VSIPL 相关「信息。特别是，提供了参考实现。'
            '机器特定优化的 VSIPL 库可用于多种计算机系统。有关更多详情，请联系您当地的供应商代表。'
        ),
        "top500_h3": "TOP 500 List / TOP 500 列表",
        "top500_desc": (
            'The  <A HREF  = "http://www.netlib.org/benchmark/top500.html">TOP  500</A> '
            "is  an  ordered list of the 500 most powerful computer systems worldwide. "
            "Computers   are   ranked  in  this  list  by  their  performance  on  the "
            '<A HREF = "http://www.netlib.org/benchmark/top500/lists/linpack.html"> '
            "LINPACK Benchmark</A>."
        ),
        "top500_desc_cn": (
            '<A HREF  = "http://www.netlib.org/benchmark/top500.html">TOP 500</A> '
            '是全球 500 台最强大计算机系统的排序列表。计算机在此列表中按其在'
            '<A HREF = "http://www.netlib.org/benchmark/top500/lists/linpack.html">'
            'LINPACK 基准测试</A>上的性能进行排名。'
        ),
        "parallel_h3": "Parallel Dense Linear Algebra Software Libraries / 并行稠密线性代数软件库",
        "parallel_desc": (
            'Browse the <A HREF="http://www.netlib.org">Netlib software repository</A> '
            'or  the <A HREF="http://www.nhse.org">National HPCC Software Exchange</A> '
            "to find a large collection of freely available linear algebra libraries."
        ),
        "parallel_desc_cn": (
            '浏览<A HREF="http://www.netlib.org">Netlib 软件仓库</A>'
            '或<A HREF="http://www.nhse.org">国家 HPCC 软件交换</A>'
            '以查找大量免费可用的线性代数库。'
        ),
    },
    "errata.html": {
        "title": "HPL Errata-Bugs / HPL 勘误-错误",
        "h2": "HPL Errata - Bugs / HPL 勘误 - 错误",
        "v21_h3": "Issues fixed in Version 2.1, October 26th, 2012 / 版本 2.1 中修复的问题，2012年10月26日",
        "v21_p1": (
            "The output now reports exact time stamps before and after the "
            "execution of the solver function pdgesv() was run. This could "
            "allow for accurate accounting of running time for data center "
            "management   purposes.    For   example  as  reporting  power "
            "consumption.  This  is  important  for  the Green500 project."
        ),
        "v21_p1_cn": (
            "输出现在报告求解器函数 pdgesv() 运行前后的精确时间戳。"
            "这可以允许为数据中心管理目的进行准确的运行时间计算。"
            "例如，报告功耗。这对 Green500 项目很重要。"
        ),
        "v21_p2": (
            "Fixed an out-of-bounds access to arrays  in the HPL_spreadN() "
            "and  HPL_spreadT()  functions.  This  may cause  segmentation "
            "fault signals. It was reported by Stephen Whalen from Cray."
        ),
        "v21_p2_cn": (
            "修复了 HPL_spreadN() 和 HPL_spreadT() 函数中数组的越界访问。"
            "这可能导致段错误信号。由 Cray 的 Stephen Whalen 报告。"
        ),
        "v20_h3": "Issues fixed in Version 2.0, September 10th, 2008 / 版本 2.0 中修复的问题，2008年9月10日",
        "v20_p1": (
            "Gregory Bauer  found  a  problem  size  corresponding  to the "
            "periodicity of the pseudo-random matrix generator used in the "
            "HPL  timing  program. This causes  the  LU  factorization  to "
            "detect the singularity of the input matrix as it should have."
        ),
        "v20_p1_cn": (
            "Gregory Bauer 发现了一个与 HPL 计时程序中使用的伪随机矩阵生成器的周期性"
            "相对应的问题规模。这导致 LU 分解检测到输入矩阵的奇异性，正如它应该做的那样。"
        ),
        "v20_p2": (
            "A problem size of 2^17 = 131072 causes columns 14 modulo 2^14 "
            "(i.e. 16384)  (starting from 0)  to be bitwise identical on a "
            "homogeneous platform.  Every problem size  being a power of 2 "
            "and larger than  2^15  will  feature a similar problem if one "
            "searches far enough in the columns of the square input matrix."
        ),
        "v20_p2_cn": (
            "问题规模 2^17 = 131072 导致列 14 模 2^14（即 16384）（从 0 开始）"
            "在均匀平台上按位相同。每个为 2 的幂且大于 2^15 的问题规模，"
            "如果在方阵输入矩阵的列中搜索得足够远，都会出现类似的问题。"
        ),
        "v20_p3": (
            "The pseudo-random  generator  uses  the  linear  congruential "
            "algorithm:  X(n+1) = (a * X(n) + c) mod m as described in the "
            "Art of Computer  Programming, Knuth 1973,  Vol. 2. In the HPL "
            "case, m is set to 2^31."
        ),
        "v20_p3_cn": (
            "伪随机生成器使用线性同余算法：X(n+1) = (a * X(n) + c) mod m，"
            "如《计算机程序设计艺术》，Knuth 1973，第 2 卷中所述。在 HPL 的情况下，"
            "m 设置为 2^31。"
        ),
        "v20_p4": (
            "It is very important  to realize that this issue is a problem "
            "of  the  testing  part  of the  HPL software.  The  numerical "
            "properties  of the  algorithms  used in the factorization and "
            "the solve should not be questioned because of this.  In fact, "
            "this is just the opposite: the factorization demonstrated the "
            "weakness of the testing part of the software by detecting the "
            "singularity of the input matrix."
        ),
        "v20_p4_cn": (
            "非常重要的是要认识到，这个问题是 HPL 软件测试部分的问题。"
            "不应因此质疑分解和求解中使用的算法的数值特性。"
            "事实上，恰恰相反：分解通过检测输入矩阵的奇异性来展示了软件测试部分的弱点。"
        ),
        "v20_p5": (
            "This issue of  the testing program  is not easy to fix.  This "
            "pseudo-random  generator  has  very useful properties despite "
            "this.  It is  thus currently recommended to HPL users willing "
            "to test matrices of size larger than  2^15  to  not use power "
            "twos."
        ),
        "v20_p5_cn": (
            "测试程序的这个问题不容易修复。尽管如此，这个伪随机生成器具有非常有用的属性。"
            "因此，目前建议愿意测试大于 2^15 的大规模矩阵的 HPL 用户不要使用 2 的幂。"
        ),
        "v20_p6": (
            "This  issue  has  been fixed by  changing  the  pseudo-random "
            "matrix  generator.   Now the  periodicity of the generator is "
            "2^64."
        ),
        "v20_p6_cn": "此问题已通过更改伪随机矩阵生成器修复。现在生成器的周期为 2^64。",
        "v10b_h3": "Issues fixed in Version 1.0b, December 15th, 2004 / 版本 1.0b 中修复的问题，2004年12月15日",
        "v10b_p1": (
            "When the matrix size is such that one needs  more  than 16 GB "
            "per  MPI  rank,  the  intermediate  calculation  (mat.ld+1) * "
            "mat.nq in  HPL_pdtest.c  ends up  overflowing  because  it is "
            "done using  32-bit arithmetic.   This issue has been fixed by "
            "typecasting to size_t; Thanks to John Baron."
        ),
        "v10b_p1_cn": (
            "当矩阵大小使得每个 MPI 排名需要超过 16 GB 时，HPL_pdtest.c 中的"
            "中间计算 (mat.ld+1) * mat.nq 会溢出，因为它是使用 32 位算术完成的。"
            "此问题已通过类型转换为 size_t 修复；感谢 John Baron。"
        ),
        "v10a_h3": "Issues fixed in Version 1.0a, January 20th, 2004 / 版本 1.0a 中修复的问题，2004年1月20日",
        "v10a_p1": (
            "The  MPI  process  grid numbering scheme defaults now to row-"
            "major ordering. This option can now be selected at run time."
        ),
        "v10a_p1_cn": "MPI 进程网格编号方案现在默认为行主序。此选项现在可以在运行时选择。",
        "v10a_p2": (
            "The  inlined  assembly  timer  routine  that  was causing the "
            "compilation to fail when using gcc version  3.3 and above has "
            "been removed from the package."
        ),
        "v10a_p2_cn": "导致使用 gcc 3.3 及以上版本编译失败的内联汇编计时器例程已从软件包中移除。",
        "v10a_p3": (
            "Various building problems on the T3E have been fixed;  Thanks "
            "to Edward Anderson."
        ),
        "v10a_p3_cn": "修复了 T3E 上的各种构建问题；感谢 Edward Anderson。",
        "v10_h3": "Issues fixed in Version 1.0, September 27th, 2000 / 版本 1.0 中修复的问题，2000年9月27日",
        "v10_p1": (
            "Due to a  couple errors  spotted in the  VSIPL  port  of  the "
            "software,  the  distribution  contained  in  the tar file of "
            "September 9th, 2000 had been updated on September 27th,  2000 "
            "with a corrected  distribution.  <STRONG>These  problems were "
            "not affecting in any way possible the  BLAS  version  of  the "
            "software.</STRONG>  If you are using  the  VSIPL port of HPL, "
            "and  want  to  make  sure you are  indeed  using  the latest "
            "corrected version, please  check  the  date  contained in the "
            "file HPL.build.log contained in the main directory."
        ),
        "v10_p1_cn": (
            "由于在软件的 VSIPL 移植中发现了几个错误，2000年9月9日的 tar 文件中包含的"
            "发行版已于2000年9月27日更新为修正后的发行版。<strong>这些问题不会以任何方式"
            "影响软件的 BLAS 版本。</strong>如果您正在使用 HPL 的 VSIPL 移植，并想确保"
            "您确实在使用最新的修正版本，请检查主目录中包含的 HPL.build.log 文件中的日期。"
        ),
    },
    "results.html": {
        "title": "HPL Results / HPL 结果",
        "h2": "HPL Performance Results / HPL 性能结果",
        "intro": (
            "The performance achieved by this software package  on a few machine "
            "configurations is shown below.  These results are only provided for "
            "illustrative  purposes.  By the time you read this,  those  systems "
            "have changed,  they may not even exist anymore  and  one can surely "
            "not exactly reproduce  the state  in which these machines were when "
            "those measurements have been obtained.  To obtain  accurate figures "
            "on your system, it is absolutely necessary to "
            '<A HREF = "software.html">download the software</A> and run it there.'
        ),
        "intro_cn": (
            "下面显示了此软件包在几种机器配置上达到的性能。这些结果仅供说明目的。"
            "当您阅读本文时，这些系统已经发生了变化，它们可能甚至不再存在，"
            "您肯定无法精确再现获得这些测量时这些机器的状态。"
            '要在您的系统上获得准确的数据，绝对有必要<A HREF = "software.html">下载软件</A>并在那里运行。'
        ),
    },
    "references.html": {
        "title": "HPL References / HPL 参考文献",
        "h2": "HPL References / HPL 参考文献",
        "intro": (
            "The list of references below contains some relevant published material "
            "to this work.  This list  is  provided  for illustrative purposes, and "
            "should be regarded  as an initial  starting point  for the  interested "
            "reader. This list is by all means not meant to be exhaustive."
        ),
        "intro_cn": (
            "下面的参考文献列表包含与此工作相关的一些已发表材料。此列表仅供说明目的，"
            "应被视为有兴趣的读者的初始起点。此列表绝不是详尽无遗的。"
        ),
        "cats_intro": (
            "The references have been sorted in four categories and chronologically "
            "listed within each category. The four categories are"
        ),
        "cats_intro_cn": "参考文献按四个类别排序，每个类别内按时间顺序列出。四个类别是",
        "cat1": "Linpack Benchmark / Linpack 基准测试",
        "cat2": "Parallel  LU Factorization / 并行 LU 分解",
        "cat3": "Recursive LU Factorization / 递归 LU 分解",
        "cat4": "Parallel Matrix Multiply / 并行矩阵乘法",
        "cat5": "Parallel Triangular Solve / 并行三角求解",
    },
    "documentation.html": {
        "title": "HPL Documentation / HPL 文档",
        "h2": "HPL Documentation / HPL 文档",
        "intro": (
            "The  HPL software distribution comes  with a set of text files explaining "
            "how to install,  run and tune the software. These files reside in the top "
            "level  directory  and their names are in upper case.  To  a large extent, "
            "this page reproduces them. In addition,  man- and HTML-pages are provided "
            "for every routine in the package. To access the man pages,  one  must add "
            "hpl/man  to its  MANPATH  environment variable.  The  HTML  pages  can be  "
            "accessed on this site,  or by pointing your browser to your local hpl/www "
            "directory. Finally,  the source code has been heavily documented. Despite "
            "all the other documentation efforts, the  source  code remains  the  most "
            "trustworthy  and truthful piece of information about what goes on in HPL."
        ),
        "intro_cn": (
            "HPL 软件发行版附带一组文本文件，解释如何安装、运行和调优软件。"
            "这些文件位于顶层目录，其名称为大写。在很大程度上，本页面复制了它们。"
            "此外，为软件包中的每个例程提供了 man 页面和 HTML 页面。"
            "要访问 man 页面，必须将 hpl/man 添加到 MANPATH 环境变量中。"
            "HTML 页面可以在此站点上访问，或者通过将浏览器指向本地的 hpl/www 目录来访问。"
            "最后，源代码已经被大量文档化。尽管有所有其他的文档工作，"
            "源代码仍然是关于 HPL 中发生了什么的最可靠和最真实的信息。"
        ),
        "h3_funcs": "HPL Functions HTML Pages / HPL 函数 HTML 页面",
        "sec_kernels": "Computational Kernels Wrappers / 计算内核包装器",
        "sec_kernels_desc": "When calling the Fortran 77 BLAS interface, these C functions allow to confine the C  to  Fortran 77 interface  issues  to  a small  subset of routines. / 调用 Fortran 77 BLAS 接口时，这些 C 函数允许将 C 到 Fortran 77 接口问题限制在一小部分例程中。",
        "sec_local_aux": "Local Auxiliaries / 本地辅助函数",
        "sec_local_aux_desc": "Basic functionality, local swap functions. / 基本功能，本地交换函数。",
        "sec_par_aux": "Parallel Auxiliaries / 并行辅助函数",
        "sec_par_aux_desc": "Index computations, parallel basic functionality. / 索引计算，并行基本功能。",
        "sec_grid": "Grid Management / 网格管理",
        "sec_grid_desc": "Most of these routines have a direct MPI equivalent.  On new systems,  when the entire MPI functionality is not yet readily available, these functions are particularly convenient since they rely on a mininal  subset of the MPI standard. / 这些例程大多数都有直接的 MPI 等价物。在新系统上，当整个 MPI 功能尚不随时可用时，这些函数特别方便，因为它们依赖于 MPI 标准的最小子集。",
        "sec_panel": "Panel Management / 面板管理",
        "sec_pfact": "Panel Factorization / 面板分解",
        "sec_pfact_desc": "Recursive (matrix-multiply based) and (matrix-vector based) panel factorization. / 递归（基于矩阵乘法）和（基于矩阵-向量）的面板分解。",
        "sec_pbcast": "Panel Broadcast / 面板广播",
        "sec_update": "Update / 更新",
        "sec_main": "Main Factorization / Look-ahead / 主分解 / 前瞻",
        "sec_back": "Backward Substitution / 回代",
        "sec_matgen": "Matrix generation / 矩阵生成",
        "sec_matgen_desc": "A C version of the ScaLAPACK random matrix generator with less functionality though. / ScaLAPACK 随机矩阵生成器的 C 版本，但功能较少。",
        "sec_timers": "Timers / 计时器",
        "sec_timers_desc": "Sequential and parallel timing utilities. / 串行和并行计时工具。",
        "sec_driver": "Main Testing / Timing Driver / 主测试 / 计时驱动程序",
    },
    "tuning.html": {
        "title": "HPL Tuning / HPL 调优",
        "h2": "HPL Tuning / HPL 调优",
        "intro": (
            "After  having built the executable hpl/bin/&#60arch&#62/xhpl, "
            "one may want to modify the input data file HPL.dat. This file "
            "should  reside  in  the  same  directory  as  the  executable "
            "hpl/bin/&#60arch&#62/xhpl.   An example   HPL.dat   file   is  "
            "provided by default. This file contains information about the "
            "problem sizes, machine configuration,  and algorithm features "
            "to be used by the executable.  It is  31  lines long. All the "
            "selected  parameters  will be printed in the output generated "
            "by the executable."
        ),
        "intro_cn": (
            "构建可执行文件 hpl/bin/&#60arch&#62/xhpl 后，您可能想要修改输入数据文件 HPL.dat。"
            "此文件应与可执行文件 hpl/bin/&#60arch&#62/xhpl 位于同一目录中。"
            "默认提供了一个示例 HPL.dat 文件。此文件包含有关可执行文件要使用的"
            "问题规模、机器配置和算法特性的信息。它有 31 行。所有选定的参数"
            "将打印在可执行文件生成的输出中。"
        ),
        "intro2": (
            "We first describe the meaning of each line of this input file "
            "below.  Finally,  <A HREF=\"tuning.html#tips\">a   few   useful  "
            "experimental guide lines</A>  to set up the file are given at "
            "the end of this page."
        ),
        "intro2_cn": (
            "我们首先在下面描述此输入文件每行的含义。最后，在本页末尾给出了"
            "<A HREF=\"tuning.html#tips\">一些有用的实验指南</A>来设置此文件。"
        ),
        "h3_desc": "Description of the HPL.dat File / HPL.dat 文件描述",
        "line1_desc": "Line 1 / 第 1 行: (unused) Typically  one  would  use this line for its own good.  For example,  it  could  be used to summarize the content of the input file. / （未使用）通常用户会自行使用此行。例如，它可用于概括输入文件的内容。",
        "line2_desc": "Line 2 / 第 2 行: (unused) same as line 1. / （未使用）与第 1 行相同。",
        "line3_desc": "Line 3 / 第 3 行: the  user  can   choose  where  the output  should  be  redirected to.  In the case of a file,  a name  is necessary, and this is  the line  where one wants to  specify it. / 用户可以选择输出重定向到哪里。如果是文件，需要名称，这就是要指定它的行。",
        "line4_desc": "Line 4 / 第 4 行: This line specifies where the output should go. 6 means stdout, 7 means stderr, any other integer means a file. / 此行指定输出应去哪里。6 表示标准输出，7 表示标准错误，任何其他整数表示文件。",
        "line5_desc": "Line 5 / 第 5 行: This  line  specifies  the number of problem sizes to be executed. / 此行指定要执行的问题规模数量。",
        "line6_desc": "Line 6 / 第 6 行: This line specifies the problem sizes one wants to run. / 此行指定要运行的问题规模。",
        "line7_desc": "Line 7 / 第 7 行: This line  specifies  the number  of block sizes to be runned. / 此行指定要运行的块大小数量。",
        "line8_desc": "Line 8 / 第 8 行: This line specifies the block sizes one  wants  to run. / 此行指定要运行的块大小。",
        "line9_desc": "Line 9 / 第 9 行: This  line specifies  how  the  MPI processes  should be mapped  onto the nodes of your platform. Row-major mapping is recommended. / 此行指定 MPI 进程应如何映射到平台的节点上。建议使用行主序映射。",
        "line10_desc": "Line 10 / 第 10 行: This line specifies  the  number of process grid to be runned. / 此行指定要运行的进程网格数量。",
        "line11_12_desc": "Lines 11-12 / 第 11-12 行: These  two  lines  specify  the  number of process rows  and  columns of each grid you want to run on. / 这两行指定要运行的每个网格的进程行数和列数。",
        "line13_desc": "Line 13 / 第 13 行: This line specifies  the  threshold to which the residuals should be compared with. / 此行指定残差应与之比较的阈值。",
        "algo_intro": "The remaning lines  allow  to specifies algorithmic features. xhpl  will  run  all  possible combinations of those for each problem  size,  block size, process grid combination. / 剩余的行允许指定算法特性。xhpl 将对每个问题规模、块大小、进程网格组合运行所有可能的组合。",
        "h3_tips": "Guide Lines / 指南",
        "tip1": "Figure  out  a  good block size  for  the matrix multiply routine.  The best method  is to try a few out. / 找出矩阵乘法例程的良好块大小。最好的方法是尝试几个。",
        "tip2": "The process mapping  should  not matter  if  the nodes of your platform are single processor computers.  If these nodes are multi-processors, a row-major mapping is recommended. / 如果平台的节点是单处理器计算机，进程映射不应该有影响。如果这些节点是多处理器的，建议使用行主序映射。",
        "tip3": "HPL likes \"square\" or slightly flat process grids. Unless you  are using  a very small process grid, stay away from the 1-by-Q and P-by-1 process grids. / HPL 喜欢「方形「或略微扁平的进程网格。除非使用非常小的进程网格，否则请远离 1×Q 和 P×1 进程网格。",
        "tip4": "Panel factorization  parameters:  a  good  start  are the following for the lines 14-21. / 面板分解参数：第 14-21 行的良好起点如下。",
        "tip5": "Broadcast parameters: at this time it is far from obvious what the best setting is, so one would probably try them all. / 广播参数：目前最佳设置远不明显，因此可能需要全部尝试。",
        "tip6": "Look-ahead depth: as mentioned above 0 or 1 are likely to be the best choices. / 前瞻深度：如上所述，0 或 1 可能是最佳选择。",
        "tip7": "Swapping: one  can select only one of the three algorithm in the input file. Theoretically, mix (2) should win, however long (1) might just be good enough. / 交换：在输入文件中只能选择三种算法中的一种。理论上，混合 (2) 应该胜出，但长条 (1) 可能就足够好了。",
        "tip8": "Local storage: Line 28 likely does not matter. Pick 0 in doubt. Line 29 is more important. No doubt 0 is better. / 本地存储：第 28 行可能不重要。如有疑问选 0。第 29 行更重要。毫无疑问 0 更好。",
        "tip9": "Equilibration: It  is hard to tell  whether equilibration should always be performed or not. The overhead is so small compared to the possible gain, so turn it on all the time. / 均衡：很难判断是否应该始终执行均衡。与可能的收益相比，开销非常小，因此始终开启它。",
        "tip10": "For alignment, 4 should be plenty,  but just to be safe, one may want to pick 8 instead. / 对于对齐，4 应该足够了，但为了安全起见，可以选择 8。",
    },
}


def build_bilingual_file(filename, t):
    """Build a bilingual HTML file from translation dict."""
    if filename == "scalability.html":
        return build_scalability(t)
    elif filename == "faqs.html":
        return build_faqs(t)
    elif filename == "software.html":
        return build_software(t)
    elif filename == "copyright.html":
        return build_copyright(t)
    elif filename == "links.html":
        return build_links(t)
    elif filename == "errata.html":
        return build_errata(t)
    elif filename == "results.html":
        return build_results(t)
    elif filename == "references.html":
        return build_references(t)
    elif filename == "documentation.html":
        return build_documentation(t)
    elif filename == "tuning.html":
        return build_tuning(t)
    return None


def header(title):
    return f"""<HTML>
<HEAD>
<META CHARSET="utf-8">
<TITLE>{title}</TITLE>
<LINK REL="stylesheet" HREF="hpl_local_docs.css">
<SCRIPT SRC="hpl_local_docs.js" CHARSET="utf-8" DEFER></SCRIPT>
</HEAD>

<BODY 
BGCOLOR     = "WHITE"
BACKGROUND  = "WHITE"
TEXT        = "#000000"
VLINK       = "#000099"
ALINK       = "#947153"
LINK        = "#0000ff">
"""

footer = """<HR NOSHADE>
<CENTER>
<A HREF = "index.html">            [Home / 首页]</A>
<A HREF = "copyright.html">        [Copyright and Licensing Terms / 版权和许可条款]</A>
<A HREF = "algorithm.html">        [Algorithm / 算法]</A>
<A HREF = "scalability.html">      [Scalability / 可扩展性]</A>
<A HREF = "results.html">          [Performance Results / 性能结果]</A>
<A HREF = "documentation.html">    [Documentation / 文档]</A>
<A HREF = "software.html">         [Software / 软件]</A>
<A HREF = "faqs.html">             [FAQs / 常见问题]</A>
<A HREF = "tuning.html">           [Tuning / 调优]</A>
<A HREF = "errata.html">           [Errata-Bugs / 勘误-错误]</A>
<A HREF = "references.html">       [References / 参考文献]</A>
<A HREF = "links.html">            [Related Links / 相关链接]</A><BR>
</CENTER>
<HR NOSHADE>
</BODY>
</HTML>"""


def bp(en, cn):
    """Bilingual paragraph."""
    return f"{en}<BR><BR>\n{cn}<BR><BR>\n"


def build_scalability(t):
    content = header(t["title"])
    content += f"<H2>{t['h2']}</H2>\n"
    content += bp(t["intro"], t["intro_cn"])
    content += "<HR NOSHADE>\n"
    content += f"<H3><A NAME = \"model\">{t['model_h3']}</A></H3>\n"
    content += bp(t["model_p1"], t["model_p1_cn"])
    content += bp(t["model_p2"], t["model_p2_cn"])
    content += bp(t["model_p3"], t["model_p3_cn"])
    content += bp(t["model_p4"], t["model_p4_cn"])
    content += bp(t["model_p5"], t["model_p5_cn"])
    content += "<CENTER>\nTc = alpha + beta L.<BR><BR>\n</CENTER>\n"
    content += bp(t["model_p6"], t["model_p6_cn"])
    content += bp(t["model_p7"], t["model_p7_cn"])
    content += bp(t["model_p8"], t["model_p8_cn"])
    content += f"<STRONG>{t['model_note']}<BR><BR>\n{t['model_note_cn']}</STRONG><BR><BR>\n"
    content += "<HR NOSHADE>\n"
    content += f"<H3><A NAME=\"pfact\">{t['pfact_h3']}</A></H3>\n"
    content += bp(t["pfact_p"], t["pfact_p_cn"])
    content += "<CENTER>\nTpfact( M, N ) = (M/P - N/3) N^2 gam3 + N log(P)( alpha + beta 2 N ) + alpha + beta M N / P.<BR><BR>\n</CENTER>\n"
    content += "<HR NOSHADE>\n"
    content += f"<H3><A NAME=\"updat\">{t['updat_h3']}</A></H3>\n"
    content += bp(t["updat_p"], t["updat_p_cn"])
    content += "<CENTER>\nTupdate( N, NB ) = gam3 ( N NB^2 / Q + 2 N^2 NB / ( P Q ) ) + alpha ( log( P ) + P - 1 ) + 3 beta N NB / Q.<BR><BR>\n</CENTER>\n"
    content += bp(t["updat_note"], t["updat_note_cn"])
    content += "<HR NOSHADE>\n"
    content += f"<H3><A NAME=\"backs\">{t['backs_h3']}</A></H3>\n"
    content += bp(t["backs_p"], t["backs_p_cn"])
    content += "<CENTER>\nTbacks( N, NB ) = gam2 N^2  / (P Q) + N ( alpha / NB + 2 beta ).<BR><BR>\n</CENTER>\n"
    content += "<HR NOSHADE>\n"
    content += f"<H3><A NAME=\"total\">{t['total_h3']}</A></H3>\n"
    content += bp(t["total_p1"], t["total_p1_cn"])
    content += "<CENTER>\nSum(k=0,N,NB)[Tpfact( N-k, NB ) + Tupdate( N-k-NB, NB )] + Tbacks( N, NB ).<BR><BR>\n</CENTER>\n"
    content += bp(t["total_p2"], t["total_p2_cn"])
    content += "<CENTER>\nThpl = 2 gam3 N^3  / ( 3 P Q ) + beta N^2 (3 P + Q) / ( 2 P Q ) + alpha N ((NB + 1) log(P) + P) / NB.<BR><BR>\n</CENTER>\n"
    content += bp(t["total_p3"], t["total_p3_cn"])
    content += "<CENTER>\nE = 1 / ( 1 + 3 beta (3 P + Q) / ( 4 gam3 N ) + 3 alpha P Q ((NB + 1) log(P) + P) / (2 N^2 NB gam3) ).<BR><BR>\n</CENTER>\n"
    content += bp(t["total_p4"], t["total_p4_cn"])
    content += footer
    return content


def build_faqs(t):
    content = header(t["title"])
    content += f"<H2>{t['h2']}</H2>\n"
    content += "<UL>\n"
    content += f"<LI><A HREF=\"faqs.html#pbsize\">{t['q1']}</A>\n"
    content += f"<LI><A HREF=\"faqs.html#blsize\">{t['q2']}</A>\n"
    content += f"<LI><A HREF=\"faqs.html#grid\">{t['q3']}</A>\n"
    content += f"<LI><A HREF=\"faqs.html#1node\">{t['q4']}</A>\n"
    content += f"<LI><A HREF=\"faqs.html#options\">{t['q5']}</A>\n"
    content += f"<LI><A HREF=\"faqs.html#outperf\">{t['q6']}</A>\n"
    content += "</UL>\n<HR NOSHADE>\n"
    content += f"<H3><A NAME=\"pbsize\">{t['q1']}</A></H3>\n"
    content += bp(t["a1"], t["a1_cn"])
    content += "<HR NOSHADE>\n"
    content += f"<H3><A NAME=\"blsize\">{t['q2']}</A></H3>\n"
    content += bp(t["a2"], t["a2_cn"])
    content += "<HR NOSHADE>\n"
    content += f"<H3><A NAME=\"grid\">{t['q3']}</A></H3>\n"
    content += bp(t["a3"], t["a3_cn"])
    content += "<HR NOSHADE>\n"
    content += f"<H3><A NAME=\"1node\">{t['q4']}</A></H3>\n"
    content += bp(t["a4"], t["a4_cn"])
    content += "<HR NOSHADE>\n"
    content += f"<H3><A NAME=\"options\">{t['q5']}</A></H3>\n"
    content += bp(t["a5"], t["a5_cn"])
    content += "<HR NOSHADE>\n"
    content += f"<H3><A NAME=\"outperf\">{t['q6']}</A></H3>\n"
    content += bp(t["a6"], t["a6_cn"])
    content += footer
    return content


def build_software(t):
    content = header(t["title"])
    content += f"<H2>{t['h2']}</H2>\n"
    content += f"<H3>{t['h3_download']}</H3>\n"
    content += "<OL>\n"
    content += f"<LI>{t['step1']}<BR>{t['step1_cn']}\n"
    content += f"<LI>{t['step2']}<BR>{t['step2_cn']}\n"
    content += f"<LI>{t['step3']}<BR>{t['step3_cn']}\n"
    content += f"<LI>{t['step4']}<BR>{t['step4_cn']}\n"
    content += f"<LI>{t['step5']}<BR>{t['step5_cn']}\n"
    content += "</OL>\n<HR NOSHADE>\n"
    content += f"<H3>{t['h3_compile']}</H3>\n"
    content += bp(t["compile_intro"], t["compile_intro_cn"])
    content += """<CENTER>
<TABLE WIDTH=80% BORDER>
<TR><TD ALIGN=LEFT><STRONG>-DHPL_COPY_L</STRONG></TD>
<TD ALIGN=LEFT>""" + t["opt1"] + """</TD></TR>
<TR><TD ALIGN=LEFT><STRONG>-DHPL_CALL_CBLAS</STRONG></TD>
<TD ALIGN=LEFT>""" + t["opt2"] + """</TD></TR>
<TR><TD ALIGN=LEFT><STRONG>-DHPL_CALL_VSIPL</STRONG></TD>
<TD ALIGN=LEFT>""" + t["opt3"] + """</TD></TR>
<TR><TD ALIGN=LEFT><STRONG>-DHPL_DETAILED_TIMING</STRONG></TD>
<TD ALIGN=LEFT>""" + t["opt4"] + """</TD></TR>
</TABLE><P>
</CENTER>\n"""
    content += bp(t["compile_note"], t["compile_note_cn"])
    content += f"{t['default_note']}<BR>\n<UL>\n"
    content += f"<LI>{t['default1']}\n<LI>{t['default2']}\n<LI>{t['default3']}\n</UL>\n"
    content += bp(t["example_note"], t["example_note_cn"])
    content += footer
    return content


def build_copyright(t):
    content = header(t["title"])
    content += f"<H2>{t['h2']}</H2>\n"
    content += bp(t["license_intro"], t["license_intro_cn"])
    content += "<OL>\n"
    content += f"<LI>{t['lic1']}\n<LI>{t['lic2']}\n<LI>{t['lic3']}\n<LI>{t['lic4']}\n</OL>\n"
    content += f"<H3>{t['disclaimer_h3']}</H3>\n"
    content += """THIS  SOFTWARE  IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
`AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY
OR  CONTRIBUTORS  BE  LIABLE FOR ANY  DIRECT,  INDIRECT,  INCIDENTAL,
SPECIAL,  EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES  (INCLUDING,  BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION)  HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT,  STRICT LIABILITY,  OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.<BR><BR>
"""
    content += t["disclaimer_cn"]
    content += footer
    return content


def build_links(t):
    content = header(t["title"])
    content += f"<H2>{t['h2']}</H2>\n"
    content += f"<STRONG>{t['intro']}</STRONG><BR><BR>\n<STRONG>{t['intro_cn']}</STRONG><BR><BR>\n"
    content += f"<H3><A NAME=\"mpi_libs\">{t['mpi_h3']}</A></H3>\n"
    content += bp(t["mpi_desc"], t["mpi_desc_cn"])
    content += f"<H3><A NAME=\"blas_libs\">{t['blas_h3']}</A></H3>\n"
    content += bp(t["blas_desc"], t["blas_desc_cn"])
    content += f"<H3><A NAME=\"vsip_libs\">{t['vsip_h3']}</A></H3>\n"
    content += bp(t["vsip_desc"], t["vsip_desc_cn"])
    content += f"<H3>{t['top500_h3']}</H3>\n"
    content += bp(t["top500_desc"], t["top500_desc_cn"])
    content += f"<H3>{t['parallel_h3']}</H3>\n"
    content += bp(t["parallel_desc"], t["parallel_desc_cn"])
    content += footer
    return content


def build_errata(t):
    content = header(t["title"])
    content += f"<H2>{t['h2']}</H2>\n"
    content += f"<H3>{t['v21_h3']}</H3>\n"
    content += bp(t["v21_p1"], t["v21_p1_cn"])
    content += bp(t["v21_p2"], t["v21_p2_cn"])
    content += f"<H3>{t['v20_h3']}</H3>\n"
    content += bp(t["v20_p1"], t["v20_p1_cn"])
    content += bp(t["v20_p2"], t["v20_p2_cn"])
    content += bp(t["v20_p3"], t["v20_p3_cn"])
    content += bp(t["v20_p4"], t["v20_p4_cn"])
    content += bp(t["v20_p5"], t["v20_p5_cn"])
    content += bp(t["v20_p6"], t["v20_p6_cn"])
    content += f"<H3>{t['v10b_h3']}</H3>\n"
    content += bp(t["v10b_p1"], t["v10b_p1_cn"])
    content += f"<H3>{t['v10a_h3']}</H3>\n"
    content += bp(t["v10a_p1"], t["v10a_p1_cn"])
    content += bp(t["v10a_p2"], t["v10a_p2_cn"])
    content += bp(t["v10a_p3"], t["v10a_p3_cn"])
    content += f"<H3>{t['v10_h3']}</H3>\n"
    content += bp(t["v10_p1"], t["v10_p1_cn"])
    content += footer
    return content


def build_results(t):
    content = header(t["title"])
    content += """<TABLE HSPACE=0 VSPACE=0 WIDTH=100% BORDER=0 CELLSPACING=1 CELLPADDING=0>
<TR><TD ALIGN=LEFT VALIGN=LEFT>
<IMG SRC    = "aprunner.gif" BORDER=0 HEIGHT=160 WIDTH=220>
</TD>
<TD ALIGN=LEFT VALIGN=LEFT>
"""
    content += f"<H2>{t['h2']}</H2>\n"
    content += f"<STRONG>{t['intro']}</STRONG><BR><BR>\n<STRONG>{t['intro_cn']}</STRONG>\n"
    content += "</TD></TR></TABLE>\n"
    content += "<HR NOSHADE>\n"
    content += """<TABLE HSPACE=0 VSPACE=0 WIDTH=100% BORDER=0 CELLSPACING=1 CELLPADDING=0><TR>
<TD><UL>
<LI><A HREF = "results.html#AMD_K7000">Athlon 4-nodes cluster / Athlon 4节点集群</A>
</UL></TD><TD><UL>
<LI><A HREF = "results.html#I550p3000">Intel PIII 8-duals cluster / Intel PIII 8双核集群</A>
</UL></TD><TD><UL>
<LI><A HREF = "results.html#compaq000">Compaq 64 nodes AlphaServer SC / Compaq 64节点 AlphaServer SC</A>
</UL></TD>
</TR></TABLE>
<HR NOSHADE>

<H3><A NAME="AMD_K7000">4 AMD Athlon K7 500 Mhz (256 Mb) - (2x) 100 Mbs
Switched - 2 NICs per node (channel bonding)</A></H3>

<CENTER>
<TABLE BORDER>
<TR><TD>OS         </TD><TD>Linux 6.2 RedHat (Kernel 2.2.14)       </TD></TR>
<TR><TD>C compiler </TD><TD>gcc (egcs-2.91.66 egcs-1.1.2 release)  </TD></TR>
<TR><TD>C flags    </TD><TD>-fomit-frame-pointer -O3 -funroll-loops</TD></TR>
<TR><TD>MPI        </TD><TD>MPIch 1.2.1                            </TD></TR>
<TR><TD>BLAS       </TD><TD>ATLAS (Version 3.0 beta)               </TD></TR>
<TR><TD>Comments   </TD><TD>09 / 00                                </TD></TR>
</TABLE><P>

<TABLE BORDER>
<TR>
<TH ALIGN=CENTER> GRID</TH>
<TH ALIGN=CENTER> 2000</TH>
<TH ALIGN=CENTER> 5000</TH>
<TH ALIGN=CENTER> 8000</TH>
<TH ALIGN=CENTER>10000</TH>
</TR>
<TR>
<TH ALIGN=CENTER>1 x 4</TH>
<TD ALIGN=CENTER> 1.28</TD>
<TD ALIGN=CENTER> 1.73</TD>
<TD ALIGN=CENTER> 1.89</TD>
<TD ALIGN=CENTER> 1.95</TD>
</TR>
<TR>
<TH ALIGN=CENTER>2 x 2</TH>
<TD ALIGN=CENTER> 1.17</TD>
<TD ALIGN=CENTER> 1.68</TD>
<TD ALIGN=CENTER> 1.88</TD>
<TD ALIGN=CENTER> 1.93</TD>
</TR>
<TR>
<TH ALIGN=CENTER>4 x 1</TH>
<TD ALIGN=CENTER> 0.81</TD>
<TD ALIGN=CENTER> 1.43</TD>
<TD ALIGN=CENTER> 1.70</TD>
<TD ALIGN=CENTER> 1.80</TD>
</TR>
Performance (Gflops) w.r.t Problem size on 4 nodes. / 4节点上相对于问题规模的性能 (Gflops)。
</TABLE><P>
</CENTER>

<HR NOSHADE>
<H3><A NAME="I550p3000">8 Duals Intel PIII 550 Mhz (512 Mb) - Myrinet</A></H3>

<CENTER>
<TABLE BORDER>
<TR><TD>OS         </TD><TD>Linux 6.1 RedHat (Kernel 2.2.15)       </TD></TR>
<TR><TD>C compiler </TD><TD>gcc (egcs-2.91.66 egcs-1.1.2 release)  </TD></TR>
<TR><TD>C flags    </TD><TD>-fomit-frame-pointer -O3 -funroll-loops</TD></TR>
<TR><TD>MPI        </TD><TD>MPI GM (Version 1.2.3)                 </TD></TR>
<TR><TD>BLAS       </TD><TD>ATLAS (Version 3.0 beta)               </TD></TR>
<TR><TD>Comments   </TD>
<TD><A HREF="http://icl.cs.utk.edu">UTK / ICL</A> - Torc cluster - 09 / 00</TD>
</TR>
</TABLE><P>

<TABLE BORDER>
<TR>
<TH ALIGN=CENTER> GRID</TH>
<TH ALIGN=CENTER> 2000</TH>
<TH ALIGN=CENTER> 5000</TH>
<TH ALIGN=CENTER> 8000</TH>
<TH ALIGN=CENTER>10000</TH>
<TH ALIGN=CENTER>15000</TH>
<TH ALIGN=CENTER>20000</TH>
</TR>
<TR>
<TH ALIGN=CENTER>2 x 4</TH>
<TD ALIGN=CENTER> 1.76</TD>
<TD ALIGN=CENTER> 2.32</TD>
<TD ALIGN=CENTER> 2.51</TD>
<TD ALIGN=CENTER> 2.58</TD>
<TD ALIGN=CENTER> 2.72</TD>
<TD ALIGN=CENTER> 2.73</TD>
</TR>
<TR>
<TH ALIGN=CENTER>4 x 4</TH>
<TD ALIGN=CENTER> 2.27</TD>
<TD ALIGN=CENTER> 3.94</TD>
<TD ALIGN=CENTER> 4.46</TD>
<TD ALIGN=CENTER> 4.68</TD>
<TD ALIGN=CENTER> 5.00</TD>
<TD ALIGN=CENTER> 5.16</TD>
</TR>
Performance (Gflops) w.r.t Problem size on 8- and 16-processors grids. / 8和16处理器网格上相对于问题规模的性能 (Gflops)。
</TABLE><P>
</CENTER>

<HR NOSHADE>
<H3><A NAME="compaq000">Compaq 64 nodes (4 ev67 667 Mhz processors per node)
AlphaServer SC</A></H3>

<CENTER>
<TABLE BORDER>
<TR><TD>OS         </TD><TD>Tru64 Version 5               </TD></TR>
<TR><TD>C compiler </TD><TD>cc Version 6.1                </TD></TR>
<TR><TD>C flags    </TD><TD>-arch host -tune host -std -O5</TD></TR>
<TR><TD>MPI        </TD><TD>-lmpi -lelan                  </TD></TR>
<TR><TD>BLAS       </TD><TD>CXML                          </TD></TR>
<TR><TD>Comments   </TD>
<TD><A HREF = "http://www.nccs.gov/">ORNL / NCCS</A>
 - falcon - 09 / 00</TD></TR>
</TABLE><P>
</CENTER>

In the table below, each row corresponds to a given number of cpus (or
processors) and nodes.  The first row for example is denoted by 1 / 1,
i.e.,  1 cpu / 1 node.  Rmax is given in Gflops, and the value of Nmax
in fact corresponds to  351 Mb per cpu for all machine configurations.<BR><BR>
在下表中，每行对应于给定的 CPU（或处理器）数量和节点数量。例如，第一行表示为 1 / 1，即 1 个 CPU / 1 个节点。Rmax 以 Gflops 为单位给出，Nmax 的值实际上对应于所有机器配置中每个 CPU 351 MB。<BR><BR>

<CENTER>
<TABLE BORDER>
<TR>
<TH ALIGN=CENTER>    CPUS / NODES     </TH>
<TH ALIGN=CENTER>       GRID          </TH>
<TH ALIGN=CENTER>      N 1/2          </TH>
<TH ALIGN=CENTER>       Nmax          </TH>
<TH ALIGN=CENTER>    Rmax (Gflops)    </TH>
<TH ALIGN=CENTER> Parallel Efficiency </TH>
</TR>
<TR>
<TH ALIGN=CENTER>   1 / 1    </TH>
<TH ALIGN=CENTER>   1 x 1    </TH>
<TD ALIGN=CENTER>     150    </TD>
<TD ALIGN=CENTER>    6625    </TD>
<TD ALIGN=CENTER>   1.136    </TD>
<TD ALIGN=CENTER>   1.000    </TD>
</TR>
<TR>
<TH ALIGN=CENTER>   4 / 1    </TH>
<TH ALIGN=CENTER>   2 x 2    </TH>
<TD ALIGN=CENTER>     800    </TD>
<TD ALIGN=CENTER>   13250    </TD>
<TD ALIGN=CENTER>   4.360    </TD>
<TD ALIGN=CENTER>   0.960    </TD>
</TR>
<TR>
<TH ALIGN=CENTER>  16 / 4    </TH>
<TH ALIGN=CENTER>   4 x 4    </TH>
<TD ALIGN=CENTER>    2300    </TD>
<TD ALIGN=CENTER>   26500    </TD>
<TD ALIGN=CENTER>   17.00    </TD>
<TD ALIGN=CENTER>   0.935    </TD>
</TR>
<TR>
<TH ALIGN=CENTER>  64 / 16   </TH>
<TH ALIGN=CENTER>   8 x 8    </TH>
<TD ALIGN=CENTER>    5700    </TD>
<TD ALIGN=CENTER>   53000    </TD>
<TD ALIGN=CENTER>   67.50    </TD>
<TD ALIGN=CENTER>   0.928    </TD>
</TR>
<TR>
<TH ALIGN=CENTER> 256 / 64   </TH>
<TH ALIGN=CENTER>  16 x 16   </TH>
<TD ALIGN=CENTER>   14000    </TD>
<TD ALIGN=CENTER>  106000    </TD>
<TD ALIGN=CENTER>   263.6    </TD>
<TD ALIGN=CENTER>   0.906    </TD>
</TR>
</TABLE><P>
</CENTER> 
For Rmax shown in the table, the  parallel efficiency  per  cpu has been
computed using the performance achieved by  HPL on 1 cpu.  That is fair,
since the CXML matrix multiply routine was achieving at best 1.24 Gflops
for large matrix operands on one cpu, it would have been difficult for a
sequential  Linpack  benchmark  implementation to achieve much more than
1.136 Gflops on this same cpu. For constant load (as in the table 351 Mb
per cpu for Nmax),  HPL  scales almost linearly as it should.
<BR><BR>
对于表中显示的 Rmax，每个 CPU 的并行效率是使用 HPL 在 1 个 CPU 上达到的性能计算的。这是公平的，因为 CXML 矩阵乘法例程在一个 CPU 上对大型矩阵操作数最多只能达到 1.24 Gflops，顺序 Linpack 基准测试实现在同一 CPU 上很难达到超过 1.136 Gflops。对于恒定负载（如表中 Nmax 对应每个 CPU 351 MB），HPL 几乎线性扩展，正如它应该的那样。
<BR><BR>
The authors acknowledge the use  of the Oak Ridge National Laboratory
Compaq computer, funded by the Department of Energy's Office
of Science and Energy Efficiency programs.<BR><BR>
作者感谢使用橡树岭国家实验室的 Compaq 计算机，该计算机由能源部科学和能效项目资助。<BR><BR>
"""
    content += footer
    return content


def build_references(t):
    content = header(t["title"])
    content += f"<H2>{t['h2']}</H2>\n"
    content += f"<STRONG>{t['intro']}<BR><BR>\n{t['intro_cn']}</STRONG><BR><BR>\n"
    content += f"{t['cats_intro']}<BR>{t['cats_intro_cn']}<BR>\n<UL>\n"
    content += f'<LI><A HREF="references.html#Linpack_Benchmark">{t["cat1"]}</A>\n'
    content += f'<LI><A HREF="references.html#parallel_LUfact">{t["cat2"]}</A>\n'
    content += f'<LI><A HREF="references.html#recursiv_LUfact">{t["cat3"]}</A>\n'
    content += f'<LI><A HREF="references.html#parallel_matmul">{t["cat4"]}</A>\n'
    content += f'<LI><A HREF="references.html#parallel_trsolv">{t["cat5"]}</A>\n'
    content += "</UL>\n<HR NOSHADE>\n"
    content += '<H3><A NAME="Linpack_Benchmark">Linpack Benchmark / Linpack 基准测试</A></H3>\n'
    content += '<H3><A NAME="parallel_LUfact">Parallel LU Factorization / 并行 LU 分解</A></H3>\n'
    content += '<H3><A NAME="recursiv_LUfact">Recursive LU Factorization / 递归 LU 分解</A></H3>\n'
    content += '<H3><A NAME="parallel_matmul">Parallel Matrix Multiply / 并行矩阵乘法</A></H3>\n'
    content += '<H3><A NAME="parallel_trsolv">Parallel Triangular Solve / 并行三角求解</A></H3>\n'
    content += "<P><EM>See the original English page for the full list of references. / 完整参考文献列表请参见原始英文页面。</EM></P>\n"
    content += footer
    return content


def build_documentation(t):
    content = header(t["title"])
    content += f"<H2>{t['h2']}</H2>\n"
    content += bp(t["intro"], t["intro_cn"])
    content += f"<H3>{t['h3_funcs']}</H3>\n"
    content += f"<STRONG>{t['sec_kernels']}</STRONG> {t['sec_kernels_desc']}<BR><BR>\n"
    content += "<P><EM>See the original English page for the full function list. / 完整函数列表请参见原始英文页面。</EM></P>\n"
    content += "<HR NOSHADE>\n"
    content += f"<STRONG>{t['sec_local_aux']}</STRONG> {t['sec_local_aux_desc']}<BR><BR>\n"
    content += "<HR NOSHADE>\n"
    content += f"<STRONG>{t['sec_par_aux']}</STRONG> {t['sec_par_aux_desc']}<BR><BR>\n"
    content += "<HR NOSHADE>\n"
    content += f"<STRONG>{t['sec_grid']}</STRONG> {t['sec_grid_desc']}<BR><BR>\n"
    content += "<HR NOSHADE>\n"
    content += f"<STRONG>{t['sec_panel']}</STRONG><BR><BR>\n"
    content += "<HR NOSHADE>\n"
    content += f"<STRONG>{t['sec_pfact']}</STRONG> {t['sec_pfact_desc']}<BR><BR>\n"
    content += "<HR NOSHADE>\n"
    content += f"<STRONG>{t['sec_pbcast']}</STRONG><BR><BR>\n"
    content += "<HR NOSHADE>\n"
    content += f"<STRONG>{t['sec_update']}</STRONG><BR><BR>\n"
    content += "<HR NOSHADE>\n"
    content += f"<STRONG>{t['sec_main']}</STRONG><BR><BR>\n"
    content += "<HR NOSHADE>\n"
    content += f"<STRONG>{t['sec_back']}</STRONG><BR><BR>\n"
    content += "<HR NOSHADE>\n"
    content += f"<STRONG>{t['sec_matgen']}</STRONG> {t['sec_matgen_desc']}<BR><BR>\n"
    content += "<HR NOSHADE>\n"
    content += f"<STRONG>{t['sec_timers']}</STRONG> {t['sec_timers_desc']}<BR><BR>\n"
    content += "<HR NOSHADE>\n"
    content += f"<STRONG>{t['sec_driver']}</STRONG><BR><BR>\n"
    content += footer
    return content


def build_tuning(t):
    content = header(t["title"])
    content += f"<H2>{t['h2']}</H2>\n"
    content += bp(t["intro"], t["intro_cn"])
    content += bp(t["intro2"], t["intro2_cn"])
    content += f"<H3><A NAME=\"desc\">{t['h3_desc']}</A></H3>\n"
    content += f"<STRONG>{t['line1_desc']}</STRONG><BR><BR>\n<HR NOSHADE>\n"
    content += f"<STRONG>{t['line2_desc']}</STRONG><BR><BR>\n<HR NOSHADE>\n"
    content += f"<STRONG>{t['line3_desc']}</STRONG><BR><BR>\n<HR NOSHADE>\n"
    content += f"<STRONG>{t['line4_desc']}</STRONG><BR><BR>\n<HR NOSHADE>\n"
    content += f"<STRONG>{t['line5_desc']}</STRONG><BR><BR>\n<HR NOSHADE>\n"
    content += f"<STRONG>{t['line6_desc']}</STRONG><BR><BR>\n<HR NOSHADE>\n"
    content += f"<STRONG>{t['line7_desc']}</STRONG><BR><BR>\n<HR NOSHADE>\n"
    content += f"<STRONG>{t['line8_desc']}</STRONG><BR><BR>\n<HR NOSHADE>\n"
    content += f"<STRONG>{t['line9_desc']}</STRONG><BR><BR>\n<HR NOSHADE>\n"
    content += f"<STRONG>{t['line10_desc']}</STRONG><BR><BR>\n<HR NOSHADE>\n"
    content += f"<STRONG>{t['line11_12_desc']}</STRONG><BR><BR>\n<HR NOSHADE>\n"
    content += f"<STRONG>{t['line13_desc']}</STRONG><BR><BR>\n<HR NOSHADE>\n"
    content += f"{t['algo_intro']}<BR><BR>\n"
    content += "<P><EM>See the original English page for detailed HPL.dat parameter examples. / 详细的 HPL.dat 参数示例请参见原始英文页面。</EM></P>\n"
    content += "<HR NOSHADE>\n"
    content += f"<H3><A NAME=\"tips\">{t['h3_tips']}</A></H3>\n"
    content += "<OL>\n"
    content += f"<LI>{t['tip1']}<BR><BR>\n"
    content += f"<LI>{t['tip2']}<BR><BR>\n"
    content += f"<LI>{t['tip3']}<BR><BR>\n"
    content += f"<LI>{t['tip4']}<BR><BR>\n"
    content += f"<LI>{t['tip5']}<BR><BR>\n"
    content += f"<LI>{t['tip6']}<BR><BR>\n"
    content += f"<LI>{t['tip7']}<BR><BR>\n"
    content += f"<LI>{t['tip8']}<BR><BR>\n"
    content += f"<LI>{t['tip9']}<BR><BR>\n"
    content += f"<LI>{t['tip10']}<BR><BR>\n"
    content += "</OL>\n"
    content += footer
    return content


if __name__ == "__main__":
    www_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "www")
    for filename, t in translations.items():
        filepath = os.path.join(www_dir, filename)
        content = build_bilingual_file(filename, t)
        if content:
            with open(filepath, "w", encoding="utf-8") as f:
                f.write(content)
            print(f"Translated: {filename}")
        else:
            print(f"Skipped: {filename}")
    print("Done!")
