<!-- Converted from HPL_lmul HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_lmul</B> multiplies 2 long positive integers.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_lmul(</CODE>
<CODE>int *</CODE>
<CODE>K</CODE>,
<CODE>int *</CODE>
<CODE>J</CODE>,
<CODE>int *</CODE>
<CODE>I</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_lmul</B>
multiplies  without carry two long positive integers K and J
and puts the result into I. The long integers  I, J, K are encoded on
64 bits using an array of 2 integers. The 32-lower bits are stored in
the first entry of each array, the 32-higher bits in the second entry
of each array. For efficiency purposes, the  intrisic modulo function
is inlined.

<H1>Arguments / 参数</H1>
<PRE>
K       (local input / 本地输入)                 int *
        On entry, K is an integer array of dimension 2 containing the
        encoded long integer K.
</PRE>
<PRE>
J       (local input / 本地输入)                 int *
        On entry, J is an integer array of dimension 2 containing the
        encoded long integer J.
</PRE>
<PRE>
I       (local output / 本地输出)                int *
        On entry, I is an integer array of dimension 2. On exit, this
        array contains the encoded long integer result.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_ladd.md">HPL_ladd</A>,
<A HREF="HPL_setran.md">HPL_setran</A>,
<A HREF="HPL_xjumpm.md">HPL_xjumpm</A>,
<A HREF="HPL_jumpit.md">HPL_jumpit</A>,
<A HREF="HPL_rand.md">HPL_rand</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
