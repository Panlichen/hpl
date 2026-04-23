<!-- Converted from HPL_pnum HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_pnum</B> Rank determination.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_pnum(</CODE>
<CODE>const HPL_T_grid *</CODE>
<CODE>GRID</CODE>,
<CODE>const int</CODE>
<CODE>MYROW</CODE>,
<CODE>const int</CODE>
<CODE>MYCOL</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_pnum</B>
determines  the  rank  of a  process  as a function  of  its
coordinates in the grid.

<H1>Arguments / 参数</H1>
<PRE>
GRID    (local input / 本地输入)                 const HPL_T_grid *
        On entry,  GRID  points  to the data structure containing the
        process grid information.<BR>进入时，GRID 指向包含进程网格信息的数据结构。
</PRE>
<PRE>
MYROW   (local input / 本地输入)                 const int
        On entry,  MYROW  specifies the row coordinate of the process
        whose rank is to be determined. MYROW must be greater than or
        equal to zero and less than NPROW.
</PRE>
<PRE>
MYCOL   (local input / 本地输入)                 const int
        On entry,  MYCOL  specifies  the  column  coordinate  of  the
        process whose rank is to be determined. MYCOL must be greater
        than or equal to zero and less than NPCOL.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_grid_init.md">HPL_grid_init</A>,
<A HREF="HPL_grid_info.md">HPL_grid_info</A>,
<A HREF="HPL_grid_exit.md">HPL_grid_exit</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
