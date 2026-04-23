<!-- Converted from HPL_plindx10 HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_plindx10</B> Compute the logarithmic maps for the spreading.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_plindx10(</CODE>
<CODE>HPL_T_panel *</CODE>
<CODE>PANEL</CODE>,
<CODE>const int</CODE>
<CODE>K</CODE>,
<CODE>const int *</CODE>
<CODE>IPID</CODE>,
<CODE>int *</CODE>
<CODE>IPLEN</CODE>,
<CODE>int *</CODE>
<CODE>IPMAP</CODE>,
<CODE>int *</CODE>
<CODE>IPMAPM1</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_plindx10</B>
computes  three arrays  IPLEN,  IPMAP  and  IPMAPM1  that
contain the logarithmic mapping information for the spreading phase.

<H1>Arguments / 参数</H1>
<PRE>
PANEL   (local input/output / 本地输入/输出)          HPL_T_panel *
        On entry,  PANEL  points to the data structure containing the
        panel information.<BR>进入时，PANEL 指向包含面板信息的数据结构。
</PRE>
<PRE>
K       (global input / 全局输入)                const int
        On entry, K specifies the number of entries in IPID.  K is at
        least 2*N, and at most 4*N.
</PRE>
<PRE>
IPID    (global input / 全局输入)                const int *
        On entry,  IPID  is an array of length K. The first K entries
        of that array contain the src and final destination resulting
        from the application of the interchanges.
</PRE>
<PRE>
IPLEN   (global output / 全局输出)               int *
        On entry, IPLEN  is an array of dimension NPROW + 1. On exit,
        this array is such that  IPLEN[i]  is the number of rows of A
        in the processes  before process IMAP[i] after the sort, with
        the convention that IPLEN[nprow] is the total number of rows.
        In other words,  IPLEN[i+1] - IPLEN[i] is the local number of
        rows of  A  that should be moved for each process.  IPLEN  is
        such that the number of rows of the source process row can be
        computed as IPLEN[1] - IPLEN[0], and the remaining entries of
        this  array are sorted  so  that  the quantities IPLEN[i+1] -
        IPLEN[i] are logarithmically sorted.
</PRE>
<PRE>
IPMAP   (global output / 全局输出)               int *
        On entry, IPMAP is an array of dimension NPROW. On exit, this
        array contains  the logarithmic mapping of the processes.  In
        other words, IPMAP[myrow] is the corresponding sorted process
        coordinate.
</PRE>
<PRE>
IPMAPM1 (global output / 全局输出)               int *
        On entry, IPMAPM1  is an array of dimension NPROW.  On  exit,
        this  array  contains  the inverse of the logarithmic mapping
        contained  in  IPMAP:  IPMAPM1[ IPMAP[i] ] = i,  for all i in
        [0.. NPROW)
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_pdlaswp00N.md">HPL_pdlaswp00N</A>,
<A HREF="HPL_pdlaswp00T.md">HPL_pdlaswp00T</A>,
<A HREF="HPL_pdlaswp01N.md">HPL_pdlaswp01N</A>,
<A HREF="HPL_pdlaswp01T.md">HPL_pdlaswp01T</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
