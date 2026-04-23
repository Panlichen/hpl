<!-- Converted from HPL_dlaswp05N HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_dlaswp05N</B> copy rows of U into A.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_dlaswp05N(</CODE>
<CODE>const int</CODE>
<CODE>M</CODE>,
<CODE>const int</CODE>
<CODE>N</CODE>,
<CODE>double *</CODE>
<CODE>A</CODE>,
<CODE>const int</CODE>
<CODE>LDA</CODE>,
<CODE>const double *</CODE>
<CODE>U</CODE>,
<CODE>const int</CODE>
<CODE>LDU</CODE>,
<CODE>const int *</CODE>
<CODE>LINDXA</CODE>,
<CODE>const int *</CODE>
<CODE>LINDXAU</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_dlaswp05N</B>
copies rows of  U of global offset LINDXAU into rows of
A at positions indicated by LINDXA.

<H1>Arguments / 参数</H1>
<PRE>
M       (local input / 本地输入)                 const int
        On entry, M  specifies the number of rows of U that should be
        copied into A. M must be at least zero.
</PRE>
<PRE>
N       (local input / 本地输入)                 const int
        On entry, N specifies the length of the rows of U that should
        be copied into A. N must be at least zero.
</PRE>
<PRE>
A       (local output / 本地输出)                double *
        On entry, A points to an array of dimension (LDA,N). On exit,
        the  rows of this array specified by  LINDXA  are replaced by
        rows of U indicated by LINDXAU.
</PRE>
<PRE>
LDA     (local input / 本地输入)                 const int
        On entry, LDA specifies the leading dimension of the array A.
        LDA must be at least MAX(1,M).
</PRE>
<PRE>
U       (local input/output / 本地输入/输出)          const double *
        On entry,  U  points to an array of dimension  (LDU,N).  This
        array contains the rows that are to be copied into A.
</PRE>
<PRE>
LDU     (local input / 本地输入)                 const int
        On entry, LDU specifies the leading dimension of the array U.
        LDU must be at least MAX(1,M).
</PRE>
<PRE>
LINDXA  (local input / 本地输入)                 const int *
        On entry, LINDXA is an array of dimension M that contains the
        local row indexes of A that should be copied from U.
</PRE>
<PRE>
LINDXAU (local input / 本地输入)                 const int *
        On entry, LINDXAU  is an array of dimension  M that  contains
        the local row indexes of U that should be copied in A.
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
