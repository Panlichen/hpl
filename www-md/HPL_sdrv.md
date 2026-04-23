<!-- Converted from HPL_sdrv HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_sdrv</B> Send and receive a message.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_sdrv(</CODE>
<CODE>double *</CODE>
<CODE>SBUF</CODE>,
<CODE>int</CODE>
<CODE>SCOUNT</CODE>,
<CODE>int</CODE>
<CODE>STAG</CODE>,
<CODE>double *</CODE>
<CODE>RBUF</CODE>,
<CODE>int</CODE>
<CODE>RCOUNT</CODE>,
<CODE>int</CODE>
<CODE>RTAG</CODE>,
<CODE>int</CODE>
<CODE>PARTNER</CODE>,
<CODE>MPI_Comm</CODE>
<CODE>COMM</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_sdrv</B>
is a simple wrapper around MPI_Sendrecv. Its main purpose is
to allow for some experimentation and tuning of this simple function.
Messages  of  length  less than  or  equal to zero  are not sent  nor
received.  Successful completion  is  indicated by the returned error
code HPL_SUCCESS.

<H1>Arguments / 参数</H1>
<PRE>
SBUF    (local input / 本地输入)                 double *
        On entry, SBUF specifies the starting address of buffer to be
        sent.
</PRE>
<PRE>
SCOUNT  (local input / 本地输入)                 int
        On entry,  SCOUNT  specifies  the number  of double precision
        entries in SBUF. SCOUNT must be at least zero.
</PRE>
<PRE>
STAG    (local input / 本地输入)                 int
        On entry,  STAG  specifies the message tag to be used for the
        sending communication operation.
</PRE>
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
RTAG    (local input / 本地输入)                 int
        On entry,  RTAG  specifies the message tag to be used for the
        receiving communication operation.
</PRE>
<PRE>
PARTNER (local input / 本地输入)                 int
        On entry,  PARTNER  specifies  the rank of the  collaborative
        process in the communication space defined by COMM.
</PRE>
<PRE>
COMM    (local input / 本地输入)                 MPI_Comm
        The MPI communicator identifying the communication space.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_send.md">HPL_send</A>,
<A HREF="HPL_recv.md">HPL_recv</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
