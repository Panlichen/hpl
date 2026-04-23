<!-- Converted from HPL_indxg2l HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_indxg2l</B> Map a global index into a local one.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>int</CODE>
<CODE>HPL_indxg2l(</CODE>
<CODE>const int</CODE>
<CODE>IG</CODE>,
<CODE>const int</CODE>
<CODE>INB</CODE>,
<CODE>const int</CODE>
<CODE>NB</CODE>,
<CODE>const int</CODE>
<CODE>SRCPROC</CODE>,
<CODE>const int</CODE>
<CODE>NPROCS</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_indxg2l</B>
computes  the local index of a matrix entry pointed to by
the  global index IG.  This  local  returned index is the same in all
processes.

<H1>Arguments / 参数</H1>
<PRE>
IG      (input)                       const int
        On entry, IG specifies the global index of the matrix  entry.
        IG must be at least zero.
</PRE>
<PRE>
INB     (input)                       const int
        On entry,  INB  specifies  the size of the first block of the
        global matrix. INB must be at least one.
</PRE>
<PRE>
NB      (input)                       const int
        On entry,  NB specifies the blocking factor used to partition
        and distribute the matrix. NB must be larger than one.
</PRE>
<PRE>
SRCPROC (input)                       const int
        On entry, if SRCPROC = -1, the data  is not  distributed  but
        replicated,  in  which  case  this  routine returns IG in all
        processes. Otherwise, the value of SRCPROC is ignored.
</PRE>
<PRE>
NPROCS  (input)                       const int
        On entry,  NPROCS  specifies the total number of process rows
        or columns over which the matrix is distributed.  NPROCS must
        be at least one.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_indxg2lp.md">HPL_indxg2lp</A>,
<A HREF="HPL_indxg2p.md">HPL_indxg2p</A>,
<A HREF="HPL_indxl2g.md">HPL_indxl2g</A>,
<A HREF="HPL_numroc.md">HPL_numroc</A>,
<A HREF="HPL_numrocI.md">HPL_numrocI</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
