<!-- Converted from HPL_pdpancrN HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_pdpancrN</B> Crout panel factorization. / Crout 面板分解。

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_pdpancrN(</CODE>
<CODE>HPL_T_panel *</CODE>
<CODE>PANEL</CODE>,
<CODE>const int</CODE>
<CODE>M</CODE>,
<CODE>const int</CODE>
<CODE>N</CODE>,
<CODE>const int</CODE>
<CODE>ICOFF</CODE>,
<CODE>double *</CODE>
<CODE>WORK</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_pdpancrN</B>
factorizes  a panel of columns that is a sub-array of a
larger one-dimensional panel  A using the Crout variant of the  usual
one-dimensional algorithm.  The lower triangular N0-by-N0 upper block
of the panel is stored in no-transpose form (i.e. just like the input
matrix itself).
 
Bi-directional  exchange  is  used  to  perform  the  swap::broadcast
operations  at once  for one column in the panel.  This  results in a
lower number of slightly larger  messages than usual.  On P processes
and assuming bi-directional links,  the running time of this function
can be approximated by (when N is equal to N0):
 
   N0 * log_2( P ) * ( lat + ( 2*N0 + 4 ) / bdwth ) +
   N0^2 * ( M - N0/3 ) * gam2-3
 
where M is the local number of rows of  the panel, lat and bdwth  are
the latency and bandwidth of the network for  double  precision  real
words, and gam2-3 is  an  estimate  of the  Level 2 and Level 3  BLAS
rate of execution. The  recursive  algorithm  allows indeed to almost
achieve  Level 3 BLAS  performance  in the panel factorization.  On a
large  number of modern machines,  this  operation is however latency
bound,  meaning  that its cost can  be estimated  by only the latency
portion N0 * log_2(P) * lat.  Mono-directional links will double this
communication cost.
 
Note that  one  iteration of the the main loop is unrolled. The local
computation of the absolute value max of the next column is performed
just after its update by the current column. This allows to bring the
current column only  once through  cache at each  step.  The  current
implementation  does not perform  any blocking  for  this sequence of
BLAS operations, however the design allows for plugging in an optimal
(machine-specific) specialized  BLAS-like kernel.  This idea has been
suggested to us by Fred Gustavson, IBM T.J. Watson Research Center.

<H1>Arguments / 参数</H1>
<PRE>
PANEL   (local input/output / 本地输入/输出)          HPL_T_panel *
        On entry,  PANEL  points to the data structure containing the
        panel information.<BR>进入时，PANEL 指向包含面板信息的数据结构。
</PRE>
<PRE>
M       (local input / 本地输入)                 const int
        On entry,  M specifies the local number of rows of sub(A).<BR>进入时，M 指定 sub(A) 的本地行数。
</PRE>
<PRE>
N       (local input / 本地输入)                 const int
        On entry,  N specifies the local number of columns of sub(A).<BR>进入时，N 指定 sub(A) 的本列数。
</PRE>
<PRE>
ICOFF   (global input / 全局输入)                const int
        On entry, ICOFF specifies the row and column offset of sub(A)
        in A.<BR>进入时，ICOFF 指定 sub(A) 在 A 中的行和列偏移量。
</PRE>
<PRE>
WORK    (local workspace / 本地工作空间)             double *
        On entry, WORK  is a workarray of size at least 2*(4+2*N0).<BR>进入时，WORK 是大小至少为 2*(4+2*N0) 的工作数组。
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_dlocmax.md">HPL_dlocmax</A>,
<A HREF="HPL_dlocswpN.md">HPL_dlocswpN</A>,
<A HREF="HPL_dlocswpT.md">HPL_dlocswpT</A>,
<A HREF="HPL_pdmxswp.md">HPL_pdmxswp</A>,
<A HREF="HPL_pdpancrT.md">HPL_pdpancrT</A>,
<A HREF="HPL_pdpanllN.md">HPL_pdpanllN</A>,
<A HREF="HPL_pdpanllT.md">HPL_pdpanllT</A>,
<A HREF="HPL_pdpanrlN.md">HPL_pdpanrlN</A>,
<A HREF="HPL_pdpanrlT.md">HPL_pdpanrlT</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
