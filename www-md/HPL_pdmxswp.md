<!-- Converted from HPL_pdmxswp HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_pdmxswp</B> swaps and broacast the pivot row.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_pdmxswp(</CODE>
<CODE>HPL_T_panel *</CODE>
<CODE>PANEL</CODE>,
<CODE>const int</CODE>
<CODE>M</CODE>,
<CODE>const int</CODE>
<CODE>II</CODE>,
<CODE>const int</CODE>
<CODE>JJ</CODE>,
<CODE>double *</CODE>
<CODE>WORK</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_pdmxswp</B>
swaps  and  broadcasts  the  absolute value max row using
bi-directional exchange.  The buffer is partially set by HPL_dlocmax.
 
Bi-directional  exchange  is  used  to  perform  the  swap::broadcast
operations  at once  for one column in the panel.  This  results in a
lower number of slightly larger  messages than usual.  On P processes
and assuming bi-directional links,  the running time of this function
can be approximated by
 
   log_2( P ) * ( lat + ( 2 * N0 + 4 ) / bdwth )
 
where  lat and bdwth are the latency and bandwidth of the network for
double precision real elements.  Communication  only  occurs  in  one
process  column. Mono-directional links  will cause the communication
cost to double.

<H1>Arguments / 参数</H1>
<PRE>
PANEL   (local input/output / 本地输入/输出)          HPL_T_panel *
        On entry,  PANEL  points to the data structure containing the
        panel information.<BR>进入时，PANEL 指向包含面板信息的数据结构。
</PRE>
<PRE>
M       (local input / 本地输入)                 const int
        On entry,  M specifies the local number of rows of the matrix
        column on which this function operates.
</PRE>
<PRE>
II      (local input / 本地输入)                 const int
        On entry, II  specifies the row offset where the column to be
        operated on starts with respect to the panel.
</PRE>
<PRE>
JJ      (local input / 本地输入)                 const int
        On entry, JJ  specifies the column offset where the column to
        be operated on starts with respect to the panel.
</PRE>
<PRE>
WORK    (local workspace / 本地工作空间)             double *
        On entry, WORK  is a workarray of size at least 2 * (4+2*N0).
        It  is assumed that  HPL_dlocmax  was called  prior  to  this
        routine to  initialize  the first four entries of this array.
        On exit, the  N0  length max row is stored in WORK[4:4+N0-1];
        Note that this is also the  JJth  row  (or column) of L1. The
        remaining part is used as a temporary array.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_dlocmax.md">HPL_dlocmax</A>,
<A HREF="HPL_dlocswpN.md">HPL_dlocswpN</A>,
<A HREF="HPL_dlocswpT.md">HPL_dlocswpT</A>,
<A HREF="HPL_pdpancrN.md">HPL_pdpancrN</A>,
<A HREF="HPL_pdpancrT.md">HPL_pdpancrT</A>,
<A HREF="HPL_pdpanllN.md">HPL_pdpanllN</A>,
<A HREF="HPL_pdpanllT.md">HPL_pdpanllT</A>,
<A HREF="HPL_pdpanrlN.md">HPL_pdpanrlN</A>,
<A HREF="HPL_pdpanrlT.md">HPL_pdpanrlT</A>,
<A HREF="HPL_pdrpancrN.md">HPL_pdrpancrN</A>,
<A HREF="HPL_pdrpancrT.md">HPL_pdrpancrT</A>,
<A HREF="HPL_pdrpanllN.md">HPL_pdrpanllN</A>,
<A HREF="HPL_pdrpanllT.md">HPL_pdrpanllT</A>,
<A HREF="HPL_pdrpanrlN.md">HPL_pdrpanrlN</A>,
<A HREF="HPL_pdrpanrlT.md">HPL_pdrpanrlT</A>,
<A HREF="HPL_pdfact.md">HPL_pdfact</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
