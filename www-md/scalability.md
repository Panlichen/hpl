<!-- Converted from HPL Scalability Analysis / HPL 可扩展性分析 -->

<H2>HPL Scalability Analysis / HPL 可扩展性分析</H2>
The <A HREF = "scalability.md#model">machine model</A> used for the analysis is first described.  This crude model is then used  to first estimate  the  parallel running time  of  the various phases  of  the algorithm namely
<UL>
<LI><A HREF="scalability.md#pfact">panel factorization and broadcast</A>,
<LI><A HREF="scalability.md#updat">trailing submatrix update</A>, 
<LI><A HREF="scalability.md#backs">backward substitution</A>. 
</UL>
Finally <A HREF="scalability.md#total">the  parallel efficiency</A> of the entire algorithm is estimated according to this machine model. We show that for a given set of parameters HPL is <STRONG>scalable</STRONG> not  only  with respect to the amount of computation,  but  also with respect to the communication volume.<BR><BR>
首先描述用于分析的<A HREF = "scalability.md#model">机器模型</A>。然后使用这个粗略模型来估计算法各阶段的并行运行时间，即
<UL>
<LI><A HREF="scalability.md#pfact">面板分解和广播</A>，
<LI><A HREF="scalability.md#updat">尾部子矩阵更新</A>，
<LI><A HREF="scalability.md#backs">回代</A>。
</UL>
最后根据此机器模型估计整个算法的<A HREF="scalability.md#total">并行效率</A>。我们表明，对于给定的参数集，HPL 不仅在计算量方面是<strong>可扩展的</strong>，而且在通信量方面也是可扩展的。<BR><BR>
<HR NOSHADE>
<H3><A NAME = "model">The Machine Model / 机器模型</A></H3>
Distributed-memory computers consist of processors that are connected using  a message passing interconnection network.  Each processor has its own memory called the local memory,  which  is accessible only to that processor.  As the time to access a remote memory is longer than the time to access a local one,  such computers are often referred to as Non-Uniform Memory Access (NUMA) machines.<BR><BR>
分布式内存计算机由通过消息传递互连网络连接的处理器组成。每个处理器都有自己的内存，称为本地内存，只能被该处理器访问。由于访问远程内存的时间比访问本地内存的时间长，这种计算机通常被称为非一致性内存访问（NUMA）机器。<BR><BR>
The interconnection network  of our machine model is static,  meaning that   it   consists  of  point-to-point  communication  links  among processors.  This  type  of  network  is also referred to as a direct network as opposed to dynamic networks.  The  latter  are constructed  from switches and communication links.  These links  are  dynamically connected  to one another by the switching elements to establish,  at run time, the paths between processors memories.<BR><BR>
我们机器模型的互连网络是静态的，即它由处理器之间的点对点通信链路组成。这种类型的网络也称为直接网络，与动态网络相对。后者由交换机和通信链路构成。这些链路由交换元件动态地相互连接，以在运行时建立处理器内存之间的路径。<BR><BR>
The  interconnection  network  of the two-dimensional  machine  model considered here is a static,  fully  connected physical topology.  It is also assumed  that  processors  can be treated  equally  in  terms of  local performance  and  that  the  communication rate between two processors depends on the processors considered.<BR><BR>
这里考虑的二维机器模型的互连网络是静态的、全连接的物理拓扑。还假设处理器在本地性能方面可以被同等对待，两个处理器之间的通信速率取决于所考虑的处理器。<BR><BR>
Our model assumes  that  a processor can send or receive data on only one of its communication ports at a time  (assuming  it has more than one). In the literature,  this  assumption is also referred to as the one-port communication model.<BR><BR>
我们的模型假设处理器一次只能在其一个通信端口上发送或接收数据（假设它有多个端口）。在文献中，这种假设也被称为单端口通信模型。<BR><BR>
The time spent to communicate  a message between two given processors is called the communication time Tc.   In  our machine model,  Tc  is approximated  by  a  linear  function  of  the number  L  of  double precision (64-bits) items communicated.  Tc is the sum of the time to prepare the message for transmission (alpha) and the time  (beta * L) taken  by the message of length  L  to traverse  the network  to  its  destination, i.e.,<BR><BR>
在两个给定处理器之间通信一条消息所花费的时间称为通信时间 Tc。在我们的机器模型中，Tc 由通信的双精度（64位）项数 L 的线性函数近似。Tc 是准备消息传输的时间（alpha）和长度为 L 的消息穿越网络到达目的地的时间（beta * L）之和，即：<BR><BR>
<CENTER>
Tc = alpha + beta L.<BR><BR>
</CENTER>
Finally,   the   model  assumes  that  the  communication  links  are bi-directional,  that is,  the time  for two processors  to send each  other a message of length L is also Tc.  A processor  can send and/or receive  a message on only one of  its communication links at a time. In particular, a processor can send a message while receiving another message from the processor it is sending to at the same time.<BR><BR>
最后，模型假设通信链路是双向的，即两个处理器互相发送长度为 L 的消息的时间也是 Tc。处理器一次只能在其一个通信链路上发送和/或接收消息。特别是，处理器可以在向某个处理器发送消息的同时从该处理器接收另一条消息。<BR><BR>
Since this document is only concerned with regular local dense linear algebra  operations,  the time taken to perform  one  floating  point  operation  is  assumed  to  be  summarized by  three constants  gam1,  gam2 and gam3. These quantitites are flop rates approximations of the vector-vector,  matrix-vector  and matrix-matrix operations for  each processor.  This  very  crude approximation summarizes all  the steps performed  by a processor  to achieve such a computation.  Obviously, such a model neglects all the phenomena  occurring  in  the processor components,  such as cache misses, pipeline startups, memory load  or store, floating point arithmetic and so on,  that  may  influence the value  of  these  constants  as a function  of the  problem size for example.<BR><BR>
由于本文档仅涉及常规的局部稠密线性代数操作，执行一次浮点操作所花费的时间假设由三个常数 gam1、gam2 和 gam3 概括。这些量是每个处理器的向量-向量、矩阵-向量和矩阵-矩阵操作的浮点速率近似。这种非常粗略的近似概括了处理器执行此类计算所执行的所有步骤。显然，这种模型忽略了处理器组件中发生的所有现象，如缓存未命中、流水线启动、内存加载或存储、浮点算术等，这些现象可能影响这些常数的值作为问题规模的函数。<BR><BR>
Similarly,  the model  does  not make any assumption on the amount of physical memory per node.  It  is  assumed that if a process has been spawn  on  a processor,  one  has  ensured  that  enough  memory  was  available  on that processor. In other words, swapping will not occur during the modeled computation.<BR><BR>
类似地，模型不对每个节点的物理内存量做任何假设。假设如果在处理器上生成了一个进程，则已确保该处理器上有足够的可用内存。换句话说，在建模的计算过程中不会发生交换。<BR><BR>
<STRONG>This  machine  model  is  a very crude approximation that is designed specifically  to  illustrate  the cost of the dominant factors of our particular case.<BR><BR>
这个机器模型是一个非常粗略的近似，专门设计用于说明我们特定情况的主要因素的成本。</STRONG><BR><BR>
<HR NOSHADE>
<H3><A NAME="pfact">Panel Factorization and Broadcast / 面板分解和广播</A></H3>
Let  consider  an  M-by-N  panel distributed over a P-process column. Because  of the recursive formulation of the panel factorization,  it is  reasonable to consider  that  the floating point operations  will be performed at matrix-matrix multiply "speed".  For  every column in the panel a binary-exchange is performed on 2*N data items. When this panel is broadcast,  what  matters  is the time that the next process column  will  spend  in this  communication operation.  Assuming  one chooses the <A HREF="algorithm.md#bcast">increasing-ring (modified) variant</A>,  only  one  message needs to be taken into account.  The execution  time  of the panel factorization and broadcast can thus be approximated by:<BR><BR>
考虑分布在 P 个进程列上的 M×N 面板。由于面板分解的递归公式，可以合理地认为浮点操作将以矩阵-矩阵乘法的“速度”执行。对于面板中的每一列，对 2*N 个数据项执行二进制交换。当此面板被广播时，重要的是下一个进程列在此通信操作中花费的时间。假设选择<A HREF="algorithm.md#bcast">递增环（修改版）变体</A>，只需要考虑一条消息。因此，面板分解和广播的执行时间可以近似为：<BR><BR>
<CENTER>
Tpfact( M, N ) = (M/P - N/3) N^2 gam3 + N log(P)( alpha + beta 2 N ) + alpha + beta M N / P.<BR><BR>
</CENTER>
<HR NOSHADE>
<H3><A NAME="updat">Trailing Submatrix Update / 尾部子矩阵更新</A></H3>
Let  consider  the  update  phase  of an  N-by-N  trailing  submatrix distributed on a P-by-Q process grid.  From  a computational point of view one has to (triangular) solve N right-hand-sides  and  perform a  local rank-NB update of this trailing submatrix. Assuming one chooses the <A HREF="algorithm.md#update">long variant</A>,  the  execution time of the update operation can be approximated by:<BR><BR>
考虑分布在 P×Q 进程网格上的 N×N 尾部子矩阵的更新阶段。从计算的角度来看，需要（三角）求解 N 个右端项，并执行此尾部子矩阵的局部秩-NB 更新。假设选择<A HREF="algorithm.md#update">长条变体</A>，更新操作的执行时间可以近似为：<BR><BR>
<CENTER>
Tupdate( N, NB ) = gam3 ( N NB^2 / Q + 2 N^2 NB / ( P Q ) ) + alpha ( log( P ) + P - 1 ) + 3 beta N NB / Q.<BR><BR>
</CENTER>
The constant "3" in front of the "beta" term is obtained  by counting one for the (logarithmic) spread phase and two for the rolling phase; In the case of bi-directional links  this constant 3 should therefore be only a 2.<BR><BR>
"beta" 项前面的常数 "3" 是通过计算（对数）散射阶段为一个和滚动阶段为两个得到的；在双向链路的情况下，此常数 3 应该仅为 2。<BR><BR>
<HR NOSHADE>
<H3><A NAME="backs">Backward Substitution / 回代</A></H3>
The number of floating point operations performed during the backward substitution in given by  N^2 / (P*Q).  Because of the lookahead, the communication cost  can be approximated at each step  by two messages of length NB, i.e.,  the time  to  communicate  the NB-piece  of  the  solution vector from one diagonal block of the matrix to another.  It follows that the execution time of the backward substitution  can  be approximated by:<BR><BR>
回代期间执行的浮点操作数为 N^2 / (P*Q)。由于前瞻，每一步的通信成本可以近似为两条长度为 NB 的消息，即从矩阵的一个对角块到另一个对角块通信解向量的 NB 片段的时间。因此，回代的执行时间可以近似为：<BR><BR>
<CENTER>
Tbacks( N, NB ) = gam2 N^2  / (P Q) + N ( alpha / NB + 2 beta ).<BR><BR>
</CENTER>
<HR NOSHADE>
<H3><A NAME="total">Putting it All Together / 综合分析</A></H3>
The total execution time of the algorithm described above is given by<BR><BR>
上述算法的总执行时间由下式给出<BR><BR>
<CENTER>
Sum(k=0,N,NB)[Tpfact( N-k, NB ) + Tupdate( N-k-NB, NB )] + Tbacks( N, NB ).<BR><BR>
</CENTER>
That is, by only considering only the dominant term in alpha, beta and gam3:<BR><BR>
即，仅考虑 alpha、beta 和 gam3 中的主导项：<BR><BR>
<CENTER>
Thpl = 2 gam3 N^3  / ( 3 P Q ) + beta N^2 (3 P + Q) / ( 2 P Q ) + alpha N ((NB + 1) log(P) + P) / NB.<BR><BR>
</CENTER>
The serial execution time is given by Tser = 2 gam3 N^3  / 3. If we define the parallel efficiency  E  as the ratio  Tser / ( P Q Thpl ), we obtain:<BR><BR>
串行执行时间由 Tser = 2 gam3 N^3 / 3 给出。如果我们将并行效率 E 定义为比率 Tser / (P Q Thpl)，我们得到：<BR><BR>
<CENTER>
E = 1 / ( 1 + 3 beta (3 P + Q) / ( 4 gam3 N ) + 3 alpha P Q ((NB + 1) log(P) + P) / (2 N^2 NB gam3) ).<BR><BR>
</CENTER>
This  last equality  shows  that when the memory usage per  processor N^2 / (P Q)  is maintained  constant, the parallel efficiency  slowly decreases  only  because of the alpha term.  The communication volume (the beta term) however remains constant.  Due to these results,  HPL is said to be <STRONG>scalable</STRONG> not only with respect  to the amount of computation,  but also  with  respect  to the communication volume.<BR><BR>
最后一个等式表明，当每个处理器的内存使用量 N^2 / (P Q) 保持恒定时，并行效率仅因为 alpha 项而缓慢下降。然而，通信量（beta 项）保持恒定。由于这些结果，HPL 被称为<strong>可扩展的</strong>，不仅在计算量方面，而且在通信量方面也是可扩展的。<BR><BR>
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
