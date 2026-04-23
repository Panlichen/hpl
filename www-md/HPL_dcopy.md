<!-- Converted from HPL_dcopy HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_dcopy</B> y := x.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_dcopy(</CODE>
<CODE>const int</CODE>
<CODE>N</CODE>,
<CODE>const double *</CODE>
<CODE>X</CODE>,
<CODE>const int</CODE>
<CODE>INCX</CODE>,
<CODE>double *</CODE>
<CODE>Y</CODE>,
<CODE>const int</CODE>
<CODE>INCY</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_dcopy</B>
copies the vector x into the vector y.

<H1>Arguments / 参数</H1>
<PRE>
N       (local input / 本地输入)                 const int
        On entry, N specifies the length of the vectors  x  and  y. N
        must be at least zero.
</PRE>
<PRE>
X       (local input / 本地输入)                 const double *
        On entry,  X  is an incremented array of dimension  at  least
        ( 1 + ( n - 1 ) * abs( INCX ) )  that  contains the vector x.
</PRE>
<PRE>
INCX    (local input / 本地输入)                 const int
        On entry, INCX specifies the increment for the elements of X.
        INCX must not be zero.
</PRE>
<PRE>
Y       (local input/output / 本地输入/输出)          double *
        On entry,  Y  is an incremented array of dimension  at  least
        ( 1 + ( n - 1 ) * abs( INCY ) )  that  contains the vector y.
        On exit, the entries of the incremented array  Y  are updated
        with the entries of the incremented array X.
</PRE>
<PRE>
INCY    (local input / 本地输入)                 const int
        On entry, INCY specifies the increment for the elements of Y.
        INCY must not be zero.
</PRE>

<H1>Example</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<PRE>
int main(int argc, char *argv[])
{
   double x[3], y[3];
   x[0] = 1.0; x[1] = 2.0; x[2] = 3.0;
   y[0] = 4.0; y[1] = 5.0; y[2] = 6.0;
   HPL_dcopy( 3, x, 1, y, 1 );
   printf("y=[%f,%f,%f]\n", y[0], y[1], y[2]);
   exit(0); return(0);
}
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_daxpy.md">HPL_daxpy</A>,
<A HREF="HPL_dscal.md">HPL_dscal</A>,
<A HREF="HPL_dswap.md">HPL_dswap</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
