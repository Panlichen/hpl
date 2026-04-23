<!-- Converted from HPL_grid_exit HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_grid_exit</B> Exit process grid.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_grid_exit(</CODE>
<CODE>HPL_T_grid *</CODE>
<CODE>GRID</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_grid_exit</B>
marks  the process  grid object for  deallocation.  The
returned  error  code  MPI_SUCCESS  indicates  successful completion.
Other error codes are (MPI) implementation dependent.

<H1>Arguments / 参数</H1>
<PRE>
GRID    (local input/output / 本地输入/输出)          HPL_T_grid *
        On entry,  GRID  points  to the data structure containing the
        process grid to be released.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_pnum.md">HPL_pnum</A>,
<A HREF="HPL_grid_init.md">HPL_grid_init</A>,
<A HREF="HPL_grid_info.md">HPL_grid_info</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
