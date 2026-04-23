<!-- Converted from HPL_pdlange HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_pdlange</B> Compute ||A||.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>double</CODE>
<CODE>HPL_pdlange(</CODE>
<CODE>const HPL_T_grid *</CODE>
<CODE>GRID</CODE>,
<CODE>const HPL_T_NORM</CODE>
<CODE>NORM</CODE>,
<CODE>const int</CODE>
<CODE>M</CODE>,
<CODE>const int</CODE>
<CODE>N</CODE>,
<CODE>const int</CODE>
<CODE>NB</CODE>,
<CODE>const double *</CODE>
<CODE>A</CODE>,
<CODE>const int</CODE>
<CODE>LDA</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_pdlange</B>
returns  the value of the one norm,  or the infinity norm,
or the element of largest absolute value of a distributed matrix A:  
 
 
   max(abs(A(i,j))) when NORM = HPL_NORM_A,                          
   norm1(A),        when NORM = HPL_NORM_1,                          
   normI(A),        when NORM = HPL_NORM_I,                          
 
where norm1 denotes the one norm of a matrix (maximum column sum) and
normI denotes  the infinity norm of a matrix (maximum row sum).  Note
that max(abs(A(i,j))) is not a matrix norm.

<H1>Arguments / 参数</H1>
<PRE>
GRID    (local input / 本地输入)                 const HPL_T_grid *
        On entry,  GRID  points  to the data structure containing the
        process grid information.<BR>进入时，GRID 指向包含进程网格信息的数据结构。
</PRE>
<PRE>
NORM    (global input / 全局输入)                const HPL_T_NORM
        On entry,  NORM  specifies  the  value to be returned by this
        function as described above.
</PRE>
<PRE>
M       (global input / 全局输入)                const int
        On entry,  M  specifies  the number  of rows of the matrix A.
        M must be at least zero.
</PRE>
<PRE>
N       (global input / 全局输入)                const int
        On entry,  N specifies the number of columns of the matrix A.
        N must be at least zero.
</PRE>
<PRE>
NB      (global input / 全局输入)                const int
        On entry,  NB specifies the blocking factor used to partition
        and distribute the matrix. NB must be larger than one.
</PRE>
<PRE>
A       (local input / 本地输入)                 const double *
        On entry,  A  points to an array of dimension  (LDA,LocQ(N)),
        that contains the local pieces of the distributed matrix A.
</PRE>
<PRE>
LDA     (local input / 本地输入)                 const int
        On entry, LDA specifies the leading dimension of the array A.
        LDA must be at least max(1,LocP(M)).
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_pdlaprnt.md">HPL_pdlaprnt</A>,
<A HREF="HPL_fprintf.md">HPL_fprintf</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
