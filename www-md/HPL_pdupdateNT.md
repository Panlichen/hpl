<!-- Converted from HPL_pdupdateNT HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_pdupdateNT</B> Broadcast a panel and update the trailing submatrix.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_pdupdateNT(</CODE>
<CODE>HPL_T_panel *</CODE>
<CODE>PBCST</CODE>,
<CODE>int *</CODE>
<CODE>IFLAG</CODE>,
<CODE>HPL_T_panel *</CODE>
<CODE>PANEL</CODE>,
<CODE>const int</CODE>
<CODE>NN</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_pdupdateNT</B>
broadcast - forward the panel PBCST and simultaneously
applies the row interchanges and updates part of the trailing  (using
the panel PANEL) submatrix.

<H1>Arguments / 参数</H1>
<PRE>
PBCST   (local input/output / 本地输入/输出)          HPL_T_panel *
        On entry,  PBCST  points to the data structure containing the
        panel (to be broadcast) information.
</PRE>
<PRE>
IFLAG   (local output / 本地输出)                int *
        On exit,  IFLAG  indicates  whether or not  the broadcast has
        been completed when PBCST is not NULL on entry. In that case,
        IFLAG is left unchanged.
</PRE>
<PRE>
PANEL   (local input/output / 本地输入/输出)          HPL_T_panel *
        On entry,  PANEL  points to the data structure containing the
        panel (to be updated) information.
</PRE>
<PRE>
NN      (local input / 本地输入)                 const int
        On entry, NN specifies  the  local  number  of columns of the
        trailing  submatrix  to be updated  starting  at the  current
        position. NN must be at least zero.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_pdgesv.md">HPL_pdgesv</A>,
<A HREF="HPL_pdgesv0.md">HPL_pdgesv0</A>,
<A HREF="HPL_pdgesvK1.md">HPL_pdgesvK1</A>,
<A HREF="HPL_pdgesvK2.md">HPL_pdgesvK2</A>,
<A HREF="HPL_pdlaswp00T.md">HPL_pdlaswp00T</A>,
<A HREF="HPL_pdlaswp01T.md">HPL_pdlaswp01T</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
