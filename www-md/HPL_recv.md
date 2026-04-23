<!-- Converted from HPL_recv HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_recv</B> Receive a message.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_recv(</CODE>
<CODE>double *</CODE>
<CODE>RBUF</CODE>,
<CODE>int</CODE>
<CODE>RCOUNT</CODE>,
<CODE>int</CODE>
<CODE>SRC</CODE>,
<CODE>int</CODE>
<CODE>RTAG</CODE>,
<CODE>MPI_Comm</CODE>
<CODE>COMM</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_recv</B>
is a simple wrapper around  MPI_Recv.  Its  main  purpose is
to  allow for some  experimentation / tuning  of this simple routine.
Successful  completion  is  indicated  by  the  returned  error  code
HPL_SUCCESS.  In the case of messages of length less than or equal to
zero, this function returns immediately.

<H1>Arguments / 参数</H1>
<PRE>
RBUF    (local output / 本地输出)                double *
        On entry, RBUF specifies the starting address of buffer to be
        received.
</PRE>
<PRE>
RCOUNT  (local input / 本地输入)                 int
        On entry,  RCOUNT  specifies  the number  of double precision
        entries in RBUF. RCOUNT must be at least zero.
</PRE>
<PRE>
SRC     (local input / 本地输入)                 int
        On entry, SRC  specifies the rank of the  sending  process in
        the communication space defined by COMM.
</PRE>
<PRE>
RTAG    (local input / 本地输入)                 int
        On entry,  STAG specifies the message tag to be used for this
        communication operation.
</PRE>
<PRE>
COMM    (local input / 本地输入)                 MPI_Comm
        The MPI communicator identifying the communication space.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_send.md">HPL_send</A>,
<A HREF="HPL_sdrv.md">HPL_sdrv</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
