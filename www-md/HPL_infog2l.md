<!-- Converted from HPL_infog2l HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_infog2l</B> global to local index translation.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_infog2l(</CODE>
<CODE>int</CODE>
<CODE>I</CODE>,
<CODE>int</CODE>
<CODE>J</CODE>,
<CODE>const int</CODE>
<CODE>IMB</CODE>,
<CODE>const int</CODE>
<CODE>MB</CODE>,
<CODE>const int</CODE>
<CODE>INB</CODE>,
<CODE>const int</CODE>
<CODE>NB</CODE>,
<CODE>const int</CODE>
<CODE>RSRC</CODE>,
<CODE>const int</CODE>
<CODE>CSRC</CODE>,
<CODE>const int</CODE>
<CODE>MYROW</CODE>,
<CODE>const int</CODE>
<CODE>MYCOL</CODE>,
<CODE>const int</CODE>
<CODE>NPROW</CODE>,
<CODE>const int</CODE>
<CODE>NPCOL</CODE>,
<CODE>int *</CODE>
<CODE>II</CODE>,
<CODE>int *</CODE>
<CODE>JJ</CODE>,
<CODE>int *</CODE>
<CODE>PROW</CODE>,
<CODE>int *</CODE>
<CODE>PCOL</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_infog2l</B>
computes the starting local index II, JJ corresponding to
the submatrix starting globally at the entry pointed by  I,  J.  This
routine returns the coordinates in the grid of the process owning the
matrix entry of global indexes I, J, namely PROW and PCOL.

<H1>Arguments / 参数</H1>
<PRE>
I       (global input / 全局输入)                int
        On entry,  I  specifies  the  global  row index of the matrix
        entry. I must be at least zero.
</PRE>
<PRE>
J       (global input / 全局输入)                int
        On entry,  J  specifies the global column index of the matrix
        entry. J must be at least zero.
</PRE>
<PRE>
IMB     (global input / 全局输入)                const int
        On entry,  IMB  specifies  the size of the first row block of
        the global matrix. IMB must be at least one.
</PRE>
<PRE>
MB      (global input / 全局输入)                const int
        On entry,  MB specifies the blocking factor used to partition
        and  distribute the rows of the matrix A.  MB  must be larger
        than one.
</PRE>
<PRE>
INB     (global input / 全局输入)                const int
        On entry, INB specifies the size of the first column block of
        the global matrix. INB must be at least one.
</PRE>
<PRE>
NB      (global input / 全局输入)                const int
        On entry,  NB specifies the blocking factor used to partition
        and distribute the columns of the matrix A. NB must be larger
        than one.
</PRE>
<PRE>
RSRC    (global input / 全局输入)                const int
        On entry,  RSRC  specifies  the row coordinate of the process
        that possesses the row  I.  RSRC  must  be at least zero  and
        strictly less than NPROW.
</PRE>
<PRE>
CSRC    (global input / 全局输入)                const int
        On entry, CSRC specifies the column coordinate of the process
        that possesses the column J. CSRC  must be at least zero  and
        strictly less than NPCOL.
</PRE>
<PRE>
MYROW   (local input / 本地输入)                 const int
        On entry, MYROW  specifies my  row process  coordinate in the
        grid. MYROW is greater than or equal  to zero  and  less than
        NPROW.
</PRE>
<PRE>
MYCOL   (local input / 本地输入)                 const int
        On entry, MYCOL specifies my column process coordinate in the
        grid. MYCOL is greater than or equal  to zero  and  less than
        NPCOL.
</PRE>
<PRE>
NPROW   (global input / 全局输入)                const int
        On entry,  NPROW  specifies the number of process rows in the
        grid. NPROW is at least one.
</PRE>
<PRE>
NPCOL   (global input / 全局输入)                const int
        On entry,  NPCOL  specifies  the number of process columns in
        the grid. NPCOL is at least one.
</PRE>
<PRE>
II      (local output / 本地输出)                int *
        On exit, II  specifies the  local  starting  row index of the
        submatrix. On exit, II is at least 0.
</PRE>
<PRE>
JJ      (local output / 本地输出)                int *
        On exit, JJ  specifies the local starting column index of the
        submatrix. On exit, JJ is at least 0.
</PRE>
<PRE>
PROW    (global output / 全局输出)               int *
        On exit, PROW is the row coordinate of the process owning the
        entry specified by the global index I.  PROW is at least zero
        and less than NPROW.
</PRE>
<PRE>
PCOL    (global output / 全局输出)               int *
        On exit, PCOL  is the column coordinate of the process owning
        the entry specified by the global index J.  PCOL  is at least
        zero and less than NPCOL.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_indxg2l.md">HPL_indxg2l</A>,
<A HREF="HPL_indxg2p.md">HPL_indxg2p</A>,
<A HREF="HPL_indxl2g.md">HPL_indxl2g</A>,
<A HREF="HPL_numroc.md">HPL_numroc</A>,
<A HREF="HPL_numrocI.md">HPL_numrocI</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
