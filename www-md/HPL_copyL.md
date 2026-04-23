<!-- Converted from HPL_copyL HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_copyL</B> Copy the current panel into a contiguous workspace.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_copyL(</CODE>
<CODE>HPL_T_panel *</CODE>
<CODE>PANEL</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_copyL</B>
copies  the  panel of columns, the L1 replicated submatrix,
the pivot array  and  the info scalar into a contiguous workspace for
later broadcast.
 
The copy of this panel  into  a contiguous buffer  can be enforced by
specifying -DHPL_COPY_L in the architecture specific Makefile.

<H1>Arguments / 参数</H1>
<PRE>
PANEL   (input/output)                HPL_T_panel *
        On entry,  PANEL  points to the  current panel data structure
        being broadcast.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_binit.md">HPL_binit</A>,
<A HREF="HPL_bcast.md">HPL_bcast</A>,
<A HREF="HPL_bwait.md">HPL_bwait</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
