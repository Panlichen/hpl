<!-- Converted from HPL_perm HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_perm</B> Combine 2 index arrays - Generate the permutation.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_perm(</CODE>
<CODE>const int</CODE>
<CODE>N</CODE>,
<CODE>int *</CODE>
<CODE>LINDXA</CODE>,
<CODE>int *</CODE>
<CODE>LINDXAU</CODE>,
<CODE>int *</CODE>
<CODE>IWORK</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_perm</B>
combines  two  index  arrays  and generate the corresponding
permutation. First, this function computes the inverse of LINDXA, and
then combine it with LINDXAU.  Second, in order to be able to perform
the permutation in place,  LINDXAU  is overwritten by the sequence of
permutation  producing  the  same result.  What we ultimately want to
achieve is:  U[LINDXAU[i]] := U[LINDXA[i]] for i in [0..N). After the
call to this function,  this in place permutation can be performed by
for i in [0..N) swap U[i] with U[LINDXAU[i]].

<H1>Arguments / 参数</H1>
<PRE>
N       (global input / 全局输入)                const int
        On entry,  N  specifies the length of the arrays  LINDXA  and
        LINDXAU. N should be at least zero.
</PRE>
<PRE>
LINDXA  (global input/output / 全局输入/输出)         int *
        On entry,  LINDXA  is an array of dimension N  containing the
        source indexes. On exit,  LINDXA  contains the combined index
        array.
</PRE>
<PRE>
LINDXAU (global input/output / 全局输入/输出)         int *
        On entry,  LINDXAU is an array of dimension N  containing the
        target indexes.  On exit,  LINDXAU  contains  the sequence of
        permutation,  that  should be applied  in increasing order to
        permute the underlying array U in place.
</PRE>
<PRE>
IWORK   (workspace)                   int *
        On entry, IWORK is a workarray of dimension N.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_plindx1.md">HPL_plindx1</A>,
<A HREF="HPL_pdlaswp01N.md">HPL_pdlaswp01N</A>,
<A HREF="HPL_pdlaswp01T.md">HPL_pdlaswp01T</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
