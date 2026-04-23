<!-- Converted from HPL_dlaswp01N HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_dlaswp01N</B> copies rows of A into itself and into U.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_dlaswp01N(</CODE>
<CODE>const int</CODE>
<CODE>M</CODE>,
<CODE>const int</CODE>
<CODE>N</CODE>,
<CODE>double *</CODE>
<CODE>A</CODE>,
<CODE>const int</CODE>
<CODE>LDA</CODE>,
<CODE>double *</CODE>
<CODE>U</CODE>,
<CODE>const int</CODE>
<CODE>LDU</CODE>,
<CODE>const int *</CODE>
<CODE>LINDXA</CODE>,
<CODE>const int *</CODE>
<CODE>LINDXAU</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_dlaswp01N</B>
copies  scattered rows  of  A  into itself  and into an
array  U.  The row offsets in  A  of the source rows are specified by
LINDXA.  The  destination of those rows are specified by  LINDXAU.  A
positive value of  LINDXAU indicates that the array destination is U,
and A otherwise.

<H1>Arguments / 参数</H1>
<PRE>
M       (local input / 本地输入)                 const int
        On entry, M  specifies the number of rows of A that should be
        moved within A or copied into U. M must be at least zero.
</PRE>
<PRE>
N       (local input / 本地输入)                 const int
        On entry, N  specifies the length of rows of A that should be
        moved within A or copied into U. N must be at least zero.
</PRE>
<PRE>
A       (local input/output / 本地输入/输出)          double *
        On entry, A points to an array of dimension (LDA,N). The rows
        of this array specified by LINDXA should be moved within A or
        copied into U.
</PRE>
<PRE>
LDA     (local input / 本地输入)                 const int
        On entry, LDA specifies the leading dimension of the array A.
        LDA must be at least MAX(1,M).
</PRE>
<PRE>
U       (local input/output / 本地输入/输出)          double *
        On entry, U points to an array of dimension (LDU,N). The rows
        of A specified by LINDXA are be copied within this array U at
        the positions indicated by positive values of LINDXAU.
</PRE>
<PRE>
LDU     (local input / 本地输入)                 const int
        On entry, LDU specifies the leading dimension of the array U.
        LDU must be at least MAX(1,M).
</PRE>
<PRE>
LINDXA  (local input / 本地输入)                 const int *
        On entry, LINDXA is an array of dimension M that contains the
        local  row indexes  of  A  that should be moved within  A  or
        or copied into U.
</PRE>
<PRE>
LINDXAU (local input / 本地输入)                 const int *
        On entry, LINDXAU  is an array of dimension  M that  contains
        the local  row indexes of  U  where the rows of  A  should be
        copied at. This array also contains the  local row offsets in
        A where some of the rows of A should be moved to.  A positive
        value of  LINDXAU[i]  indicates that the row  LINDXA[i]  of A
        should be copied into U at the position LINDXAU[i]; otherwise
        the row  LINDXA[i]  of  A  should be moved  at  the  position
        -LINDXAU[i] within A.
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
