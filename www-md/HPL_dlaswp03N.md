<!-- Converted from HPL_dlaswp03N HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_dlaswp03N</B> copy rows of W into U.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_dlaswp03N(</CODE>
<CODE>const int</CODE>
<CODE>M</CODE>,
<CODE>const int</CODE>
<CODE>N</CODE>,
<CODE>double *</CODE>
<CODE>U</CODE>,
<CODE>const int</CODE>
<CODE>LDU</CODE>,
<CODE>const double *</CODE>
<CODE>W0</CODE>,
<CODE>const double *</CODE>
<CODE>W</CODE>,
<CODE>const int</CODE>
<CODE>LDW</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_dlaswp03N</B>
copies columns of  W  into  rows  of an  array  U.  The
destination in U of these columns contained in W is stored within W0.

<H1>Arguments / 参数</H1>
<PRE>
M       (local input / 本地输入)                 const int
        On entry, M  specifies  the  number  of columns of  W  stored
        contiguously that should be copied into U. M must be at least
        zero.
</PRE>
<PRE>
N       (local input / 本地输入)                 const int
        On entry,  N  specifies  the  length of columns of  W  stored
        contiguously that should be copied into U. N must be at least
        zero.
</PRE>
<PRE>
U       (local input/output / 本地输入/输出)          double *
        On entry, U points to an array of dimension (LDU,N).  Columns
        of W are copied as rows within this array U at  the positions
        specified in W0.
</PRE>
<PRE>
LDU     (local input / 本地输入)                 const int
        On entry, LDU specifies the leading dimension of the array U.
        LDU must be at least MAX(1,M).
</PRE>
<PRE>
W0      (local input / 本地输入)                 const double *
        On entry,  W0  is an array of size (M-1)*LDW+1, that contains
        the destination offset  in U where the columns of W should be
        copied.
</PRE>
<PRE>
W       (local input / 本地输入)                 const double *
        On entry, W  is an array of size (LDW,M),  that contains data
        to be copied into U. For i in [0..M),  entries W(:,i)  should
        be copied into the row or column W0(i*LDW) of U.
</PRE>
<PRE>
LDW     (local input / 本地输入)                 const int
        On entry, LDW specifies the leading dimension of the array W.
        LDW must be at least MAX(1,N+1).
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_dlaswp00N.md">HPL_dlaswp00N</A>,
<A HREF="HPL_dlaswp10N.md">HPL_dlaswp10N</A>,
<A HREF="HPL_dlaswp01N.md">HPL_dlaswp01N</A>,
<A HREF="HPL_dlaswp01T.md">HPL_dlaswp01T</A>,
<A HREF="HPL_dlaswp02N.md">HPL_dlaswp02N</A>,
<A HREF="HPL_dlaswp03N.md">HPL_dlaswp03N</A>,
<A HREF="HPL_dlaswp03T.md">HPL_dlaswp03T</A>,
<A HREF="HPL_dlaswp04N.md">HPL_dlaswp04N</A>,
<A HREF="HPL_dlaswp04T.md">HPL_dlaswp04T</A>,
<A HREF="HPL_dlaswp05N.md">HPL_dlaswp05N</A>,
<A HREF="HPL_dlaswp05T.md">HPL_dlaswp05T</A>,
<A HREF="HPL_dlaswp06N.md">HPL_dlaswp06N</A>,
<A HREF="HPL_dlaswp06T.md">HPL_dlaswp06T</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
