<!-- Converted from HPL_barrier HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_barrier</B> Barrier operation.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_barrier(</CODE>
<CODE>MPI_Comm</CODE>
<CODE>COMM</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_barrier</B>
blocks the caller until all process members have call it.
The  call  returns  at any process  only after all group members have
entered the call.

<H1>Arguments / 参数</H1>
<PRE>
COMM    (global/local input)          MPI_Comm
        The MPI communicator identifying the process collection.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_broadcast.md">HPL_broadcast</A>,
<A HREF="HPL_reduce.md">HPL_reduce</A>,
<A HREF="HPL_all_reduce.md">HPL_all_reduce</A>,
<A HREF="HPL_min.md">HPL_min</A>,
<A HREF="HPL_max.md">HPL_max</A>,
<A HREF="HPL_sum.md">HPL_sum</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
