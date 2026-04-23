<!-- Converted from HPL_send HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_send</B> Send a message.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_send(</CODE>
<CODE>double *</CODE>
<CODE>SBUF</CODE>,
<CODE>int</CODE>
<CODE>SCOUNT</CODE>,
<CODE>int</CODE>
<CODE>DEST</CODE>,
<CODE>int</CODE>
<CODE>STAG</CODE>,
<CODE>MPI_Comm</CODE>
<CODE>COMM</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_send</B>
is a simple wrapper around  MPI_Send.  Its  main  purpose is
to  allow for some  experimentation / tuning  of this simple routine.
Successful  completion  is  indicated  by  the  returned  error  code
MPI_SUCCESS.  In the case of messages of length less than or equal to
zero, this function returns immediately.

<H1>Arguments / 参数</H1>
<PRE>
SBUF    (local input / 本地输入)                 double *
        On entry, SBUF specifies the starting address of buffer to be
        sent.
</PRE>
<PRE>
SCOUNT  (local input / 本地输入)                 int
        On entry,  SCOUNT  specifies  the number of  double precision
        entries in SBUF. SCOUNT must be at least zero.
</PRE>
<PRE>
DEST    (local input / 本地输入)                 int
        On entry, DEST specifies the rank of the receiving process in
        the communication space defined by COMM.
</PRE>
<PRE>
STAG    (local input / 本地输入)                 int
        On entry,  STAG specifies the message tag to be used for this
        communication operation.
</PRE>
<PRE>
COMM    (local input / 本地输入)                 MPI_Comm
        The MPI communicator identifying the communication space.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_recv.md">HPL_recv</A>,
<A HREF="HPL_sdrv.md">HPL_sdrv</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
