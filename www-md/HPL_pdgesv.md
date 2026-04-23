<!-- Converted from HPL_pdgesv HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_pdgesv</B> Solve A x = b. / 求解 A x = b。

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_pdgesv(</CODE>
<CODE>HPL_T_grid *</CODE>
<CODE>GRID</CODE>,
<CODE>HPL_T_palg *</CODE>
<CODE>ALGO</CODE>,
<CODE>HPL_T_pmat *</CODE>
<CODE>A</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_pdgesv</B>
factors a N+1-by-N matrix using LU factorization with row
partial pivoting.  The main algorithm  is the "right looking" variant
with  or  without look-ahead.  The  lower  triangular  factor is left
unpivoted and the pivots are not returned. The right hand side is the
N+1 column of the coefficient matrix.

<H1>Arguments / 参数</H1>
<PRE>
GRID    (local input / 本地输入)                 HPL_T_grid *
        On entry,  GRID  points  to the data structure containing the
        process grid information.<BR>进入时，GRID 指向包含进程网格信息的数据结构。
</PRE>
<PRE>
ALGO    (global input / 全局输入)                HPL_T_palg *
        On entry,  ALGO  points to  the data structure containing the
        algorithmic parameters.<BR>进入时，ALGO 指向包含算法参数的数据结构。
</PRE>
<PRE>
A       (local input/output / 本地输入/输出)          HPL_T_pmat *
        On entry, A points to the data structure containing the local
        array information.<BR>进入时，A 指向包含本地数组信息的数据结构。
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_pdgesv0.md">HPL_pdgesv0</A>,
<A HREF="HPL_pdgesvK1.md">HPL_pdgesvK1</A>,
<A HREF="HPL_pdgesvK2.md">HPL_pdgesvK2</A>,
<A HREF="HPL_pdtrsv.md">HPL_pdtrsv</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
