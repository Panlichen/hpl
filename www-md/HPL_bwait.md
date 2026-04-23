<!-- Converted from HPL_bwait HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_bwait</B> Finalize the row broadcast.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_bwait(</CODE>
<CODE>HPL_T_panel *</CODE>
<CODE>PANEL</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_bwait</B>
HPL_bwait waits  for  the  row  broadcast  of  the current  panel  to
terminate.  Successful completion is indicated by the returned  error
code HPL_SUCCESS.

<H1>Arguments / 参数</H1>
<PRE>
PANEL   (input/output)                HPL_T_panel *
        On entry,  PANEL  points to the  current panel data structure
        being broadcast.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_binit.md">HPL_binit</A>,
<A HREF="HPL_bcast.md">HPL_bcast</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
