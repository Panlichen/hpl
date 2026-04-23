<!-- Converted from HPL_packL HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_packL</B> Form the MPI structure for the row ring broadcasts.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_packL(</CODE>
<CODE>HPL_T_panel *</CODE>
<CODE>PANEL</CODE>,
<CODE>const int</CODE>
<CODE>INDEX</CODE>,
<CODE>const int</CODE>
<CODE>LEN</CODE>,
<CODE>const int</CODE>
<CODE>IBUF</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_packL</B>
forms  the MPI data type for the panel to be broadcast.
Successful  completion  is  indicated  by  the  returned  error  code
MPI_SUCCESS.

<H1>Arguments / 参数</H1>
<PRE>
PANEL   (input/output)                HPL_T_panel *
        On entry,  PANEL  points to the  current panel data structure
        being broadcast.
</PRE>
<PRE>
INDEX   (input)                       const int
        On entry,  INDEX  points  to  the  first entry of the  packed
        buffer being broadcast.
</PRE>
<PRE>
LEN     (input)                       const int
        On entry, LEN is the length of the packed buffer.
</PRE>
<PRE>
IBUF    (input)                       const int
        On entry, IBUF  specifies the panel buffer/count/type entries
        that should be initialized.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_binit.md">HPL_binit</A>,
<A HREF="HPL_bcast.md">HPL_bcast</A>,
<A HREF="HPL_bwait.md">HPL_bwait</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
