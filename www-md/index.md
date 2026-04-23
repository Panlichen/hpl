<!-- Converted from HPL - A Portable Implementation of the High-Performance Linpack Benchmark for Distributed-Memory Computers -->

<HR NOSHADE>
<TABLE HSPACE=0 VSPACE=0 WIDTH=100% BORDER=0 CELLSPACING=1 CELLPADDING=0>
<TR>
<TD ALIGN=CENTER>
<H3>HPL - A Portable Implementation of the High-Performance Linpack
Benchmark for Distributed-Memory Computers<BR>
HPL - 高性能Linpack基准测试的分布式内存计算机可移植实现</H3>
</TD>
<TD ALIGN=LEFT VALIGN=LEFT>
<A HREF     = "http://icl.cs.utk.edu">
<IMG SRC    = 2-273x48.jpg
ALT         = "ICL - UTK Computer Science Department"
BORDER      = 0
HEIGHT      = 48
WIDTH       = 273></A>
</TD>
</TR>
</TABLE>
<TABLE HSPACE=0 VSPACE=0 WIDTH=100% BORDER=0 CELLSPACING=1 CELLPADDING=0>
<TR>
<TD ALIGN=LEFT>Version 2.2</TD>
<TD ALIGN=CENTER>
<A HREF     = "http://www.cs.utk.edu/~petitet">A. Petitet</A>,
<A HREF     = "http://www.cs.utk.edu/~rwhaley">R. C. Whaley</A>,
<A HREF     = "http://www.netlib.org/utk/people/JackDongarra">J. Dongarra</A>,
<A HREF     = "mailto:cleary1@llnl.gov">A. Cleary</A>
</TD>
<TD ALIGN=CENTER>December 2, 2018</TD>
<TD ALIGN=RIGHT>
<A HREF="http://www.netlib.org/master_counts2.html#benchmark/hpl"># Accesses</A>
</TD>
</TR>
</TABLE>
<HR NOSHADE><BR> 

<STRONG>HPL</STRONG> is  a software  package  that solves  a (random)
dense  linear  system  in  double  precision   (64  bits)  arithmetic
on  distributed-memory  computers.    It  can  thus  be  regarded  as
a portable as well as  freely available  implementation  of the  High
Performance Computing Linpack Benchmark.<BR><BR>
<STRONG>HPL</STRONG> 是一个软件包，用于在分布式内存计算机上以双精度（64位）算术求解（随机）稠密线性方程组。因此，它可以被视为高性能计算Linpack基准测试的一个可移植且免费可用的实现。<BR><BR>

The <STRONG>algorithm</STRONG> used  by HPL  can be summarized by the
following keywords:  Two-dimensional  block-cyclic data  distribution
- Right-looking variant  of  the  LU  factorization with row partial
pivoting  featuring  multiple  look-ahead depths  -  Recursive  panel
factorization  with  pivot  search  and  column  broadcast combined -
Various  virtual  panel  broadcast topologies  -  bandwidth  reducing
swap-broadcast  algorithm -  backward  substitution  with  look-ahead
of depth 1.<BR><BR>
HPL 使用的<strong>算法</strong>可以通过以下关键词概括：二维块循环数据分布 - 采用行部分主元法、具有多前瞻深度的 LU 分解的右视变体 - 结合主元搜索和列广播的递归面板分解 - 多种虚拟面板广播拓扑 - 带宽减少的交换广播算法 - 深度为 1 的前瞻回代。<BR><BR>

The  HPL package  provides  a testing and timing program  to quantify
the  <STRONG>accuracy</STRONG> of  the obtained solution  as  well as
the time it took to compute it. The best <STRONG>performance</STRONG>
achievable by this software on your system depends on a large variety
of factors.  Nonetheless,  with some restrictive assumptions  on  the
interconnection  network,   the  algorithm  described  here  and  its
attached implementation  are <STRONG>scalable</STRONG>  in  the sense
that their parallel efficiency is maintained  constant  with  respect
to the per processor memory usage.<BR><BR>
HPL 软件包提供了一个测试和计时程序，用于量化所获得解的<strong>精度</strong>以及计算它所花费的时间。此软件在您的系统上可达到的最佳<strong>性能</strong>取决于多种因素。尽管如此，在对互连网络做一些限制性假设的情况下，这里描述的算法及其附带的实现是<strong>可扩展的</strong>，即它们的并行效率相对于每个处理器的内存使用量保持恒定。<BR><BR>

The HPL software package <STRONG>requires</STRONG>  the  availibility
on your system of an implementation of the  Message Passing Interface
<STRONG>MPI</STRONG> (1.1 compliant).
An implementation of <STRONG>either</STRONG> the Basic Linear Algebra
Subprograms   <STRONG>BLAS  or</STRONG>   the   Vector  Signal  Image
Processing Library <STRONG>VSIPL</STRONG> is also needed.
Machine-specific as well as generic implementations of
<A HREF = "links.md#mpi_libs">MPI</A>, the
<A HREF = "links.md#blas_libs">BLAS</A> and
<A HREF = "links.md#vsip_libs">VSIPL</A> are available  for a large
variety of systems.<BR><BR>
HPL 软件包<strong>要求</strong>您的系统上提供消息传递接口 <STRONG>MPI</STRONG>（1.1 兼容）的实现。还需要 <STRONG>BLAS</STRONG>（基本线性代数子程序）<STRONG>或</STRONG> <STRONG>VSIPL</STRONG>（向量信号图像处理库）的实现。<A HREF = "links.md#mpi_libs">MPI</A>、<A HREF = "links.md#blas_libs">BLAS</A> 和 <A HREF = "links.md#vsip_libs">VSIPL</A> 的机器特定实现和通用实现可用于多种系统。<BR><BR>

<STRONG>Acknowledgements</STRONG>: This work was  supported  in  part
by  a  grant  from  the  Department  of  Energy's   Lawrence
Livermore National Laboratory  and  Los  Alamos  National  Laboratory
as   part  of  the   ASCI  Projects   contract  numbers  B503962  and
12187-001-00 4R.
<BR><BR>
<strong>致谢</strong>：本工作部分由能源部劳伦斯利弗莫尔国家实验室和洛斯阿拉莫斯国家实验室的资助支持，作为 ASCI 项目的一部分，合同号为 B503962 和 12187-001-00 4R。

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

<ADDRESS>
Innovative Computing Laboratory<BR>
last revised December 2, 2018<BR>
创新计算实验室<BR>
最后修订：2018年12月2日<BR>
</ADDRESS>

<PRE>
#########################################################################

file    <a href="hpl-2.3.tar.gz">hpl-2.3.tar.gz</a>
for     HPL 2.3 - A Portable Implementation of the High-Performance Linpack
,       Benchmark for Distributed-Memory Computers 
by      Antoine Petitet, Clint Whaley, Jack Dongarra, Andy Cleary, Piotr Luszczek
Updated: December 2, 2018

#########################################################################

file    <a href="hpl-2.2.tar.gz">hpl-2.2.tar.gz</a>
for     HPL 2.2 - A Portable Implementation of the High-Performance Linpack
,       Benchmark for Distributed-Memory Computers 
by      Antoine Petitet, Clint Whaley, Jack Dongarra, Andy Cleary, Piotr Luszczek
Updated: February 24, 2016

#########################################################################

file    <a href="hpl-2.1.tar.gz">hpl-2.1.tar.gz</a>
for     HPL 2.1 - A Portable Implementation of the High-Performance Linpack
,       Benchmark for Distributed-Memory Computers 
by      Antoine Petitet, Clint Whaley, Jack Dongarra, Andy Cleary, Piotr Luszczek
Updated: October 26, 2012

#########################################################################

file    <a href="hpl-2.0.tar.gz">hpl-2.0.tar.gz</a>
for     HPL 2.0 - A Portable Implementation of the High-Performance Linpack
,       Benchmark for Distributed-Memory Computers 
by      Antoine Petitet, Clint Whaley, Jack Dongarra, Andy Cleary
Updated: September 10, 2008

#########################################################################

file    <a href="hpl.tgz">hpl.tgz</a>
for     HPL 1.0a - A Portable Implementation of the High-Performance Linpack
,       Benchmark for Distributed-Memory Computers 
by      Antoine Petitet, Clint Whaley, Jack Dongarra, Andy Cleary
Updated: January 20, 2004<BR>

#########################################################################

file    <a href="hpl_qs22-2008-11-30.patch">hpl_qs22-2008-11-30.patch</a>
for     Implementation of the High-Performance Linpack benchmark for IBM
,       QS22 systems with PowerXCell 8i processors. The file is a patch
,       for HPL 1.0a.
by      IBM

file    <a href="IBM_LICENSE.TXT">IBM_LICENSE.TXT</a>
for     IBM Copyright notice for QS22 HPL
by      IBM

file    <a href="IBM_README.txt">IBM_README.txt</a>
for     README for IBM QS22 HPL
by      IBM
Updated: November 30, 2008


#########################################################################
</PRE>
