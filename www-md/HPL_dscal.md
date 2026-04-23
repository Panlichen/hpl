<!-- Converted from HPL_dscal HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_dscal</B> x = alpha * x.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_dscal(</CODE>
<CODE>const int</CODE>
<CODE>N</CODE>,
<CODE>const double</CODE>
<CODE>ALPHA</CODE>,
<CODE>double *</CODE>
<CODE>X</CODE>,
<CODE>const int</CODE>
<CODE>INCX</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_dscal</B>
scales the vector x by alpha.

<H1>Arguments / 参数</H1>
<PRE>
N       (local input / 本地输入)                 const int
        On entry, N specifies the length of the vector x. N  must  be
        at least zero.
</PRE>
<PRE>
ALPHA   (local input / 本地输入)                 const double
        On entry, ALPHA specifies the scalar alpha.   When  ALPHA  is
        supplied as zero, then the entries of the incremented array X
        need not be set on input.
</PRE>
<PRE>
X       (local input/output / 本地输入/输出)          double *
        On entry,  X  is an incremented array of dimension  at  least
        ( 1 + ( n - 1 ) * abs( INCX ) )  that  contains the vector x.
        On exit, the entries of the incremented array  X  are  scaled
        by the scalar alpha.
</PRE>
<PRE>
INCX    (local input / 本地输入)                 const int
        On entry, INCX specifies the increment for the elements of X.
        INCX must not be zero.
</PRE>

<H1>Example</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<PRE>
int main(int argc, char *argv[])
{
   double x[3];
   x[0] = 1.0; x[1] = 2.0; x[2] = 3.0;
   HPL_dscal( 3, 2.0, x, 1 );
   printf("x=[%f,%f,%f]\n", x[0], x[1], x[2]);
   exit(0); return(0);
}
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_daxpy.md">HPL_daxpy</A>,
<A HREF="HPL_dcopy.md">HPL_dcopy</A>,
<A HREF="HPL_dswap.md">HPL_dswap</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
