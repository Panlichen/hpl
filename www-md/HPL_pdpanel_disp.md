<!-- Converted from HPL_pdpanel_disp HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_pdpanel_disp</B> Deallocate a panel data structure.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_pdpanel_disp(</CODE>
<CODE>HPL_T_panel * *</CODE>
<CODE>PANEL</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_pdpanel_disp</B>
deallocates  the  panel  structure  and  resources  and
stores the error code returned by the panel factorization.

<H1>Arguments / 参数</H1>
<PRE>
PANEL   (local input/output / 本地输入/输出)          HPL_T_panel * *
        On entry,  PANEL  points  to  the  address  of the panel data
        structure to be deallocated.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_pdpanel_new.md">HPL_pdpanel_new</A>,
<A HREF="HPL_pdpanel_init.md">HPL_pdpanel_init</A>,
<A HREF="HPL_pdpanel_free.md">HPL_pdpanel_free</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
