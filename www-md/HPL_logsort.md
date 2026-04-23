<!-- Converted from HPL_logsort HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_logsort</B> Sort the processes in logarithmic order.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_logsort(</CODE>
<CODE>const int</CODE>
<CODE>NPROCS</CODE>,
<CODE>const int</CODE>
<CODE>ICURROC</CODE>,
<CODE>int *</CODE>
<CODE>IPLEN</CODE>,
<CODE>int *</CODE>
<CODE>IPMAP</CODE>,
<CODE>int *</CODE>
<CODE>IPMAPM1</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_logsort</B>
computes an array  IPMAP  and  its inverse  IPMAPM1  that
contain  the logarithmic sorted processes id with repect to the local
number of rows of  U  that they own. This is necessary to ensure that
the logarithmic spreading of U is optimal in terms of number of steps
and communication volume as well.  In other words,  the larget pieces
of U will be sent a minimal number of times.

<H1>Arguments / 参数</H1>
<PRE>
NPROCS  (global input / 全局输入)                const int
        On entry, NPROCS  specifies the number of process rows in the
        process grid. NPROCS is at least one.
</PRE>
<PRE>
ICURROC (global input / 全局输入)                const int
        On entry, ICURROC is the source process row.
</PRE>
<PRE>
IPLEN   (global input/output / 全局输入/输出)         int *
        On entry, IPLEN is an array of dimension NPROCS+1,  such that
        IPLEN[0] is 0, and IPLEN[i] contains the number of rows of U,
        that process i-1 has.  On exit,  IPLEN[i]  is  the number  of
        rows of U  in the processes before process IPMAP[i] after the
        sort,  with  the convention that  IPLEN[NPROCS] is  the total
        number  of rows  of the panel.  In other words,  IPLEN[i+1] -
        IPLEN[i] is  the  number of rows of A that should be moved to
        the process IPMAP[i].  IPLEN  is such that the number of rows
        of  the  source process  row is IPLEN[1] - IPLEN[0],  and the
        remaining  entries  of  this  array  are  sorted  so that the
        quantities IPLEN[i+1]-IPLEN[i] are logarithmically sorted.
</PRE>
<PRE>
IPMAP   (global output / 全局输出)               int *
        On entry,  IPMAP  is an array of dimension  NPROCS.  On exit,
        array contains  the logarithmic mapping of the processes.  In
        other words, IPMAP[myroc] is the corresponding sorted process
        coordinate.
</PRE>
<PRE>
IPMAPM1 (global output / 全局输出)               int *
        On entry, IPMAPM1  is an array of dimension NPROCS.  On exit,
        this  array  contains  the inverse of the logarithmic mapping
        contained  in  IPMAP:  IPMAPM1[ IPMAP[i] ] = i,  for all i in
        [0.. NPROCS)
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_plindx1.md">HPL_plindx1</A>,
<A HREF="HPL_plindx10.md">HPL_plindx10</A>,
<A HREF="HPL_pdlaswp01N.md">HPL_pdlaswp01N</A>,
<A HREF="HPL_pdlaswp01T.md">HPL_pdlaswp01T</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
