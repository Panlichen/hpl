<!-- Converted from HPL_grid_init HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_grid_init</B> Create a process grid.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_grid_init(</CODE>
<CODE>MPI_Comm</CODE>
<CODE>COMM</CODE>,
<CODE>const HPL_T_ORDER</CODE>
<CODE>ORDER</CODE>,
<CODE>const int</CODE>
<CODE>NPROW</CODE>,
<CODE>const int</CODE>
<CODE>NPCOL</CODE>,
<CODE>HPL_T_grid *</CODE>
<CODE>GRID</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_grid_init</B>
creates a NPROW x NPCOL  process  grid using column- or
row-major ordering from an initial collection of processes identified
by an  MPI  communicator.  Successful  completion is indicated by the
returned error code MPI_SUCCESS.  Other error codes depend on the MPI
implementation. The coordinates of processes that are not part of the
grid are set to values outside of [0..NPROW) x [0..NPCOL).

<H1>Arguments / 参数</H1>
<PRE>
COMM    (global/local input)          MPI_Comm
        On entry,  COMM  is  the  MPI  communicator  identifying  the
        initial  collection  of  processes out of which  the  grid is
        formed.
</PRE>
<PRE>
ORDER   (global input / 全局输入)                const HPL_T_ORDER
        On entry, ORDER specifies how the processes should be ordered
        in the grid as follows:
           ORDER = HPL_ROW_MAJOR    row-major    ordering;
           ORDER = HPL_COLUMN_MAJOR column-major ordering;
</PRE>
<PRE>
NPROW   (global input / 全局输入)                const int
        On entry,  NPROW  specifies the number of process rows in the
        grid to be created. NPROW must be at least one.
</PRE>
<PRE>
NPCOL   (global input / 全局输入)                const int
        On entry,  NPCOL  specifies  the number of process columns in
        the grid to be created. NPCOL must be at least one.
</PRE>
<PRE>
GRID    (local input/output / 本地输入/输出)          HPL_T_grid *
        On entry,  GRID  points  to the data structure containing the
        process grid information to be initialized.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_pnum.md">HPL_pnum</A>,
<A HREF="HPL_grid_info.md">HPL_grid_info</A>,
<A HREF="HPL_grid_exit.md">HPL_grid_exit</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
