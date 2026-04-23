<!-- Converted from HPL_reduce HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_reduce</B> Reduce operation.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_reduce(</CODE>
<CODE>void *</CODE>
<CODE>BUFFER</CODE>,
<CODE>const int</CODE>
<CODE>COUNT</CODE>,
<CODE>const HPL_T_TYPE</CODE>
<CODE>DTYPE</CODE>,
<CODE>const HPL_T_OP </CODE>
<CODE>OP</CODE>,
<CODE>const int</CODE>
<CODE>ROOT</CODE>,
<CODE>MPI_Comm</CODE>
<CODE>COMM</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_reduce</B>
performs a global reduce operation across all processes of
a group.  Note that the input buffer is  used as workarray and in all
processes but the accumulating process corrupting the original data.

<H1>Arguments / 参数</H1>
<PRE>
BUFFER  (local input/output / 本地输入/输出)          void *
        On entry,  BUFFER  points to  the  buffer to be  reduced.  On
        exit,  and  in process of rank  ROOT  this array contains the
        reduced data.  This  buffer  is also used as workspace during
        the operation in the other processes of the group.
</PRE>
<PRE>
COUNT   (global input / 全局输入)                const int
        On entry,  COUNT  indicates the number of entries in  BUFFER.
        COUNT must be at least zero.
</PRE>
<PRE>
DTYPE   (global input / 全局输入)                const HPL_T_TYPE
        On entry,  DTYPE  specifies the type of the buffers operands.
</PRE>
<PRE>
OP      (global input / 全局输入)                const HPL_T_OP 
        On entry, OP is a pointer to the local combine function.
</PRE>
<PRE>
ROOT    (global input / 全局输入)                const int
        On entry, ROOT is the coordinate of the accumulating process.
</PRE>
<PRE>
COMM    (global/local input)          MPI_Comm
        The MPI communicator identifying the process collection.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_broadcast.md">HPL_broadcast</A>,
<A HREF="HPL_all_reduce.md">HPL_all_reduce</A>,
<A HREF="HPL_barrier.md">HPL_barrier</A>,
<A HREF="HPL_min.md">HPL_min</A>,
<A HREF="HPL_max.md">HPL_max</A>,
<A HREF="HPL_sum.md">HPL_sum</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
