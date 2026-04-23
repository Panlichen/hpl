<!-- Converted from HPL Frequently Asked Questions / HPL 常见问题 -->

<H2>HPL Frequently Asked Questions / HPL 常见问题</H2>
<UL>
<LI><A HREF="faqs.md#pbsize">What problem size N should I run ? / 我应该运行多大的问题规模 N ？</A>
<LI><A HREF="faqs.md#blsize">What block size NB should I use ? / 我应该使用什么块大小 NB ？</A>
<LI><A HREF="faqs.md#grid">What process grid ratio P x Q should I use ? / 我应该使用什么进程网格比例 P × Q ？</A>
<LI><A HREF="faqs.md#1node">What about the one processor case ? / 单处理器情况如何 ？</A>
<LI><A HREF="faqs.md#options">Why so many options in HPL.dat ? / 为什么 HPL.dat 中有这么多选项 ？</A>
<LI><A HREF="faqs.md#outperf">Can HPL be Outperformed ? / HPL 可以被超越吗 ？</A>
</UL>
<HR NOSHADE>
<H3><A NAME="pbsize">What problem size N should I run ? / 我应该运行多大的问题规模 N ？</A></H3>
In order  to find out  the  best performance   of  your  system,  the largest   problem size  fitting in memory is what you should aim for. The  amount  of  memory  used  by  HPL is essentially the size of the  coefficient matrix.  So for example, if you have 4 nodes  with 256 Mb of memory on each, this corresponds to 1 Gb total, i.e., 125 M double precision  (8  bytes)  elements. The  square  root  of that number is 11585.  One  definitely needs to leave some memory for the OS as well as for other things, so a problem size of 10000 is likely to fit.  As a rule of thumb, 80 % of the  total amount of memory is a good guess. If the problem size you pick is too large,  swapping will occur,  and the performance will drop.  If multiple processes  are spawn  on each node  (say  you have 2 processors  per  node),  what  counts  is  the available amount of memory to each process.<BR><BR>
为了找出系统的最佳性能，您应该以适合内存的最大问题规模为目标。HPL 使用的内存量基本上是系数矩阵的大小。例如，如果您有 4 个节点，每个有 256 MB 内存，这对应于总共 1 GB，即 125M 个双精度（8 字节）元素。该数字的平方根为 11585。肯定需要为操作系统和其他东西留出一些内存，因此问题规模 10000 可能适合。作为经验法则，总内存量的 80% 是一个好的猜测。如果您选择的问题规模太大，将发生交换，性能将下降。如果每个节点上生成了多个进程（例如每个节点有 2 个处理器），重要的是每个进程可用的内存量。<BR><BR>
<HR NOSHADE>
<H3><A NAME="blsize">What block size NB should I use ? / 我应该使用什么块大小 NB ？</A></H3>
HPL  uses  the block size NB for the data distribution as well as for the  computational  granularity.  From  a data distribution  point of view,  the smallest NB,  the better the load balance.  You definitely want  to stay away  from very large values of NB.  From a computation point of view,  a too small value of NB  may  limit the computational performance by a large factor because almost no data reuse will occur in the highest level of the memory hierarchy. The  number of messages will  also  increase.  Efficient  matrix-multiply  routines are often  internally  blocked.  Small  multiples  of  this  blocking factor are  likely to be good block sizes for HPL. The bottom line is that "good" block sizes are almost always in the [32 .. 256] interval.  The  best values depend on the computation / communication performance ratio of your system. To a much less extent, the problem size matters as well. Say for example,  you emperically found that 44 was a good block size with respect to performance.  88 or 132  are likely  to give slightly  better results  for large problem sizes because of a slighlty  higher flop rate.<BR><BR>
HPL 使用块大小 NB 进行数据分布和计算粒度。从数据分布的角度来看，NB 越小，负载平衡越好。您一定要避免使用非常大的 NB 值。从计算的角度来看，NB 值太小可能会大幅限制计算性能，因为在内存层次结构的最高层几乎不会发生数据重用。消息数量也会增加。高效的矩阵乘法例程通常在内部是分块的。此分块因子的小倍数可能是 HPL 的良好块大小。底线是「好」的块大小几乎总是在 [32 .. 256] 区间内。最佳值取决于系统的计算/通信性能比。问题规模的影响要小得多。例如，如果您凭经验发现 44 是一个性能良好的块大小，那么 88 或 132 可能会在大问题规模上给出略好的结果，因为浮点速率略高。<BR><BR>
<HR NOSHADE>
<H3><A NAME="grid">What process grid ratio P x Q should I use ? / 我应该使用什么进程网格比例 P × Q ？</A></H3>
This  depends  on  the  physical  interconnection  network  you have. Assuming a mesh or a switch HPL "likes" a 1:k ratio with k in [1..3]. In  other  words,  P  and  Q  should  be approximately equal,  with Q  slightly larger than P. Examples: 2 x 2, 2 x 4, 2 x 5,  3 x 4, 4 x 4, 4 x 6, 5 x 6, 4 x 8 ...  If  you  are  running  on  a simple Ethernet  network,  there  is only one wire through which all the messages are exchanged. On  such a network, the performance and scalability of HPL is strongly limited  and very flat process grids are likely to be the best choices: 1 x 4, 1 x 8, 2 x 4 ...<BR><BR>
这取决于您的物理互连网络。假设是网格或交换机，HPL 「喜欢」 1:k 的比例，其中 k 在 [1..3] 范围内。换句话说，P 和 Q 应该大致相等，Q 略大于 P。例如：2×2, 2×4, 2×5, 3×4, 4×4, 4×6, 5×6, 4×8 ...如果您在简单的以太网上运行，只有一条线路用于交换所有消息。在这样的网络上，HPL 的性能和可扩展性受到严重限制，非常扁平的进程网格可能是最佳选择：1×4, 1×8, 2×4 ...<BR><BR>
<HR NOSHADE>
<H3><A NAME="1node">What about the one processor case ? / 单处理器情况如何 ？</A></H3>
HPL  has  been  designed  to  perform well for large problem sizes on hundreds  of  nodes and more.  The software works on one node and for large problem sizes, one  can usually achieve pretty good performance on a single processor as well.  For small problem sizes  however, the overhead  due  to  message-passing,  local  indexing and so on can be  significant.<BR><BR>
HPL 的设计目标是在数百个节点或更多节点上对大问题规模表现良好。该软件可以在一个节点上运行，对于大问题规模，通常也可以在单个处理器上获得相当好的性能。然而，对于小问题规模，消息传递、本地索引等开销可能很显著。<BR><BR>
<HR NOSHADE>
<H3><A NAME="options">Why so many options in HPL.dat ? / 为什么 HPL.dat 中有这么多选项 ？</A></H3>
There are quite a few reasons. First off, these options are useful to determine what matters and what does not on your system. Second,  HPL is often used in the context  of early evaluation of new systems.  In such a case, everything is usually not quite working right, and it is convenient  to be able  to vary these parameters without recompiling. Finally,  every system has its own peculiarities and one is likely to be  willing  to  emperically determine the best set of parameters. In any   case,  one  can  always  follow  the  advice  provided  in  the <A HREF = "tuning.md">tuning  section</A> of this  document and not worry about the complexity of the input file.<BR><BR>
有很多原因。首先，这些选项有助于确定在您的系统上什么重要什么不重要。其次，HPL 经常在新系统的早期评估中使用。在这种情况下，通常一切都不太正常，能够不重新编译就改变这些参数是很方便的。最后，每个系统都有自己的特点，用户可能愿意通过实验确定最佳的参数集。无论如何，您可以始终遵循本文档<A HREF = "tuning.md">调优部分</A>中提供的建议，而不必担心输入文件的复杂性。<BR><BR>
<HR NOSHADE>
<H3><A NAME="outperf">Can HPL be Outperformed ? / HPL 可以被超越吗 ？</A></H3>
Certainly.   There  is  always  room  for  performance  improvements. Specific knowledge about  a  particular system  is always a source of performance   gains.  Even  from  a generic  point  of  view,  better algorithms  or  more  efficient  formulation  of the classic ones are potential winners.<BR><BR>
当然。性能改进总是有空间的。对特定系统的专门知识总是性能提升的来源。即使从通用角度来看，更好的算法或经典算法的更高效公式化也可能是赢家。<BR><BR>
<HR NOSHADE>
<CENTER>
<A HREF = "index.md">            [Home / 首页]</A>
<A HREF = "copyright.md">        [Copyright and Licensing Terms / 版权和许可条款]</A>
<A HREF = "algorithm.md">        [Algorithm / 算法]</A>
<A HREF = "scalability.md">      [Scalability / 可扩展性]</A>
<A HREF = "results.md">          [Performance Results / 性能结果]</A>
<A HREF = "documentation.md">    [Documentation / 文档]</A>
<A HREF = "software.md">         [Software / 软件]</A>
<A HREF = "faqs.md">             [FAQs / 常见问题]</A>
<A HREF = "tuning.md">           [Tuning / 调优]</A>
<A HREF = "errata.md">           [Errata-Bugs / 勘误-错误]</A>
<A HREF = "references.md">       [References / 参考文献]</A>
<A HREF = "links.md">            [Related Links / 相关链接]</A><BR>
</CENTER>
<HR NOSHADE>
