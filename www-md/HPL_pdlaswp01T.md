<!-- Converted from HPL_pdlaswp01T HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_pdlaswp01T</B> Broadcast a column panel L and swap the row panel U.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_pdlaswp01T(</CODE>
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
<B>HPL_pdlaswp01T</B>
applies the  NB  row interchanges to  NN columns of the
trailing submatrix and broadcast a column panel.
 
A "Spread then roll" algorithm performs  the swap :: broadcast  of the
row panel U at once,  resulting in a minimal communication volume  and
a "very good"  use of the connectivity if available.  With  P  process
rows  and  assuming  bi-directional links,  the  running time  of this
function can be approximated by:
 
   (log_2(P)+(P-1)) * lat +   K * NB * LocQ(N) / bdwth
 
where  NB  is the number of rows of the row panel U,  N is the global
number of columns being updated,  lat and bdwth  are the latency  and
bandwidth  of  the  network  for  double  precision real words.  K is
a constant in (2,3] that depends on the achieved bandwidth  during  a
simultaneous  message exchange  between two processes.  An  empirical
optimistic value of K is typically 2.4.

<H1>Arguments / 参数</H1>
<PRE>
PBCST   (local input/output / 本地输入/输出)          HPL_T_panel *
        On entry,  PBCST  points to the data structure containing the
        panel (to be broadcast) information.
</PRE>
<PRE>
IFLAG   (local input/output / 本地输入/输出)          int *
        On entry, IFLAG  indicates  whether or not  the broadcast has
        already been completed.  If not,  probing will occur, and the
        outcome will be contained in IFLAG on exit.
</PRE>
<PRE>
PANEL   (local input/output / 本地输入/输出)          HPL_T_panel *
        On entry,  PANEL  points to the data structure containing the
        panel information.<BR>进入时，PANEL 指向包含面板信息的数据结构。
</PRE>
<PRE>
NN      (local input / 本地输入)                 const int
        On entry, NN specifies  the  local  number  of columns of the
        trailing  submatrix  to  be swapped and broadcast starting at
        the current position. NN must be at least zero.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_pdgesv.md">HPL_pdgesv</A>,
<A HREF="HPL_pdgesvK2.md">HPL_pdgesvK2</A>,
<A HREF="HPL_pdupdateNT.md">HPL_pdupdateNT</A>,
<A HREF="HPL_pdupdateTT.md">HPL_pdupdateTT</A>,
<A HREF="HPL_pipid.md">HPL_pipid</A>,
<A HREF="HPL_plindx1.md">HPL_plindx1</A>,
<A HREF="HPL_plindx10.md">HPL_plindx10</A>,
<A HREF="HPL_spreadT.md">HPL_spreadT</A>,
<A HREF="HPL_equil.md">HPL_equil</A>,
<A HREF="HPL_rollT.md">HPL_rollT</A>,
<A HREF="HPL_dlaswp10N.md">HPL_dlaswp10N</A>,
<A HREF="HPL_dlaswp01T.md">HPL_dlaswp01T</A>,
<A HREF="HPL_dlaswp06T.md">HPL_dlaswp06T</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
