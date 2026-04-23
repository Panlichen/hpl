<!-- Converted from HPL_pdtrsv HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_pdtrsv</B> Solve triu( A ) x = b.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_pdtrsv(</CODE>
<CODE>HPL_T_grid *</CODE>
<CODE>GRID</CODE>,
<CODE>HPL_T_pmat *</CODE>
<CODE>AMAT</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_pdtrsv</B>
solves an upper triangular system of linear equations.
 
The rhs is the last column of the N by N+1 matrix A. The solve starts
in the process  column owning the  Nth  column of A, so the rhs b may
need to be moved one process column to the left at the beginning. The
routine therefore needs  a column  vector in every process column but
the one owning  b. The result is  replicated in all process rows, and
returned in XR, i.e. XR is of size nq = LOCq( N ) in all processes.
 
The algorithm uses decreasing one-ring broadcast in process rows  and
columns  implemented  in terms of  synchronous communication point to
point primitives.  The  lookahead of depth 1 is used to minimize  the
critical path. This entire operation is essentially ``latency'' bound
and an estimate of its running time is given by:
 
   (move rhs) lat + N / ( P bdwth ) +            
   (solve)    ((N / NB)-1) 2 (lat + NB / bdwth) +
              gam2 N^2 / ( P Q ),                
 
where  gam2   is an estimate of the   Level 2 BLAS rate of execution.
There are  N / NB  diagonal blocks. One must exchange  2  messages of
length NB to compute the next  NB  entries of the vector solution, as
well as performing a total of N^2 floating point operations.

<H1>Arguments / 参数</H1>
<PRE>
GRID    (local input / 本地输入)                 HPL_T_grid *
        On entry,  GRID  points  to the data structure containing the
        process grid information.<BR>进入时，GRID 指向包含进程网格信息的数据结构。
</PRE>
<PRE>
AMAT    (local input/output / 本地输入/输出)          HPL_T_pmat *
        On entry,  AMAT  points  to the data structure containing the
        local array information.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_pdgesv.md">HPL_pdgesv</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
