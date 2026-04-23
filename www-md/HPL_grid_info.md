<!-- Converted from HPL_grid_info HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_grid_info</B> Retrieve grid information.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_grid_info(</CODE>
<CODE>const HPL_T_grid *</CODE>
<CODE>GRID</CODE>,
<CODE>int *</CODE>
<CODE>NPROW</CODE>,
<CODE>int *</CODE>
<CODE>NPCOL</CODE>,
<CODE>int *</CODE>
<CODE>MYROW</CODE>,
<CODE>int *</CODE>
<CODE>MYCOL</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_grid_info</B>
returns  the grid shape and the coordinates in the grid
of the calling process.  Successful  completion  is  indicated by the
returned error code  MPI_SUCCESS. Other error codes depend on the MPI
implementation.

<H1>Arguments / 参数</H1>
<PRE>
GRID    (local input / 本地输入)                 const HPL_T_grid *
        On entry,  GRID  points  to the data structure containing the
        process grid information.<BR>进入时，GRID 指向包含进程网格信息的数据结构。
</PRE>
<PRE>
NPROW   (global output / 全局输出)               int *
        On exit,   NPROW  specifies the number of process rows in the
        grid. NPROW is at least one.
</PRE>
<PRE>
NPCOL   (global output / 全局输出)               int *
        On exit,   NPCOL  specifies  the number of process columns in
        the grid. NPCOL is at least one.
</PRE>
<PRE>
MYROW   (global output / 全局输出)               int *
        On exit,  MYROW  specifies my  row process  coordinate in the
        grid. MYROW is greater than or equal  to zero  and  less than
        NPROW.
</PRE>
<PRE>
MYCOL   (global output / 全局输出)               int *
        On exit,  MYCOL specifies my column process coordinate in the
        grid. MYCOL is greater than or equal  to zero  and  less than
        NPCOL.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_pnum.md">HPL_pnum</A>,
<A HREF="HPL_grid_init.md">HPL_grid_init</A>,
<A HREF="HPL_grid_exit.md">HPL_grid_exit</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
