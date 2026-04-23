<!-- Converted from HPL_pdpanel_free HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_pdpanel_free</B> Deallocate the panel ressources.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_pdpanel_free(</CODE>
<CODE>HPL_T_panel *</CODE>
<CODE>PANEL</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_pdpanel_free</B>
deallocates  the panel resources  and  stores the error
code returned by the panel factorization.

<H1>Arguments / 参数</H1>
<PRE>
PANEL   (local input/output / 本地输入/输出)          HPL_T_panel *
        On entry,  PANEL  points  to  the  panel data  structure from
        which the resources should be deallocated.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_pdpanel_new.md">HPL_pdpanel_new</A>,
<A HREF="HPL_pdpanel_init.md">HPL_pdpanel_init</A>,
<A HREF="HPL_pdpanel_disp.md">HPL_pdpanel_disp</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
