<!-- Converted from HPL_rand HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_rand</B> random number generator.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>double</CODE>
<CODE>HPL_rand();</CODE>

<H1>Description / 描述</H1>
<B>HPL_rand</B>
generates  the next number  in the  random  sequence.  This
function  ensures  that this number lies in the interval (-0.5, 0.5].
 
The static array irand contains the information (2 integers) required
to generate the  next number  in the sequence  X(n).  This  number is
computed as X(n) = (2^32 * irand[1] + irand[0]) / d - 0.5,  where the
constant d is the largest 64 bit positive integer. The array irand is
then  updated  for the generation of the next number  X(n+1)  in  the
random sequence as follows X(n+1) = a * X(n) + c. The constants a and
c  should have been preliminarily stored in the arrays ias and ics as
2 pairs of integers.  The initialization of  ias,  ics and  irand  is
performed by the function HPL_setran.

<H1>See Also / 另见</H1>
<A HREF="HPL_ladd.md">HPL_ladd</A>,
<A HREF="HPL_lmul.md">HPL_lmul</A>,
<A HREF="HPL_setran.md">HPL_setran</A>,
<A HREF="HPL_xjumpm.md">HPL_xjumpm</A>,
<A HREF="HPL_jumpit.md">HPL_jumpit</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
