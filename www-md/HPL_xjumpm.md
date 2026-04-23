<!-- Converted from HPL_xjumpm HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_xjumpm</B> Compute constants to jump in the random sequence.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_xjumpm(</CODE>
<CODE>const int</CODE>
<CODE>JUMPM</CODE>,
<CODE>int *</CODE>
<CODE>MULT</CODE>,
<CODE>int *</CODE>
<CODE>IADD</CODE>,
<CODE>int *</CODE>
<CODE>IRANN</CODE>,
<CODE>int *</CODE>
<CODE>IRANM</CODE>,
<CODE>int *</CODE>
<CODE>IAM</CODE>,
<CODE>int *</CODE>
<CODE>ICM</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_xjumpm</B>
computes  the constants  A and C  to jump JUMPM numbers in
the random sequence: X(n+JUMPM) = A*X(n)+C.  The constants encoded in
MULT and IADD  specify  how to jump from one entry in the sequence to
the next.

<H1>Arguments / 参数</H1>
<PRE>
JUMPM   (local input / 本地输入)                 const int
        On entry,  JUMPM  specifies  the  number  of entries  in  the
        sequence to jump over. When JUMPM is less or equal than zero,
        A and C are not computed, IRANM is set to IRANN corresponding
        to a jump of size zero.
</PRE>
<PRE>
MULT    (local input / 本地输入)                 int *
        On entry, MULT is an array of dimension 2,  that contains the
        16-lower  and 15-higher bits of the constant  a  to jump from
        X(n) to X(n+1) = a*X(n) + c in the random sequence.
</PRE>
<PRE>
IADD    (local input / 本地输入)                 int *
        On entry, IADD is an array of dimension 2,  that contains the
        16-lower  and 15-higher bits of the constant  c  to jump from
        X(n) to X(n+1) = a*X(n) + c in the random sequence.
</PRE>
<PRE>
IRANN   (local input / 本地输入)                 int *
        On entry, IRANN is an array of dimension 2. that contains the
        16-lower and 15-higher bits of the encoding of X(n).
</PRE>
<PRE>
IRANM   (local output / 本地输出)                int *
        On entry,  IRANM  is an array of dimension 2.   On exit, this
        array  contains respectively  the 16-lower and 15-higher bits
        of the encoding of X(n+JUMPM).
</PRE>
<PRE>
IAM     (local output / 本地输出)                int *
        On entry, IAM is an array of dimension 2. On exit, when JUMPM
        is  greater  than  zero,  this  array  contains  the  encoded
        constant  A  to jump from  X(n) to  X(n+JUMPM)  in the random
        sequence. IAM(0:1)  contains  respectively  the  16-lower and
        15-higher  bits  of this constant  A. When  JUMPM  is less or
        equal than zero, this array is not referenced.
</PRE>
<PRE>
ICM     (local output / 本地输出)                int *
        On entry, ICM is an array of dimension 2. On exit, when JUMPM
        is  greater  than  zero,  this  array  contains  the  encoded
        constant  C  to jump from  X(n)  to  X(n+JUMPM) in the random
        sequence. ICM(0:1)  contains  respectively  the  16-lower and
        15-higher  bits  of this constant  C. When  JUMPM  is less or
        equal than zero, this array is not referenced.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_ladd.md">HPL_ladd</A>,
<A HREF="HPL_lmul.md">HPL_lmul</A>,
<A HREF="HPL_setran.md">HPL_setran</A>,
<A HREF="HPL_jumpit.md">HPL_jumpit</A>,
<A HREF="HPL_rand.md">HPL_rand</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
