<!-- Converted from HPL_dgemv HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_dgemv</B> y := beta * y + alpha * op(A) * x.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_dgemv(</CODE>
<CODE>const enum HPL_ORDER</CODE>
<CODE>ORDER</CODE>,
<CODE>const enum HPL_TRANS</CODE>
<CODE>TRANS</CODE>,
<CODE>const int</CODE>
<CODE>M</CODE>,
<CODE>const int</CODE>
<CODE>N</CODE>,
<CODE>const double</CODE>
<CODE>ALPHA</CODE>,
<CODE>const double *</CODE>
<CODE>A</CODE>,
<CODE>const int</CODE>
<CODE>LDA</CODE>,
<CODE>const double *</CODE>
<CODE>X</CODE>,
<CODE>const int</CODE>
<CODE>INCX</CODE>,
<CODE>const double</CODE>
<CODE>BETA</CODE>,
<CODE>double *</CODE>
<CODE>Y</CODE>,
<CODE>const int</CODE>
<CODE>INCY</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_dgemv</B>
performs one of the matrix-vector operations
 
    y := alpha * op( A ) * x + beta * y,
 
 where op( X ) is one of
 
    op( X ) = X   or   op( X ) = X^T.
 
where alpha and beta are scalars, x and y are vectors and  A  is an m
by n matrix.

<H1>Arguments / 参数</H1>
<PRE>
ORDER   (local input / 本地输入)                 const enum HPL_ORDER
        On entry, ORDER  specifies the storage format of the operands
        as follows:                                                  
           ORDER = HplRowMajor,                                      
           ORDER = HplColumnMajor.                                   
</PRE>
<PRE>
TRANS   (local input / 本地输入)                 const enum HPL_TRANS
        On entry,  TRANS  specifies the  operation to be performed as
        follows:   
           TRANS = HplNoTrans y := alpha*A  *x + beta*y,
           TRANS = HplTrans   y := alpha*A^T*x + beta*y.
</PRE>
<PRE>
M       (local input / 本地输入)                 const int
        On entry,  M  specifies  the number of rows of  the matrix A.
        M must be at least zero.
</PRE>
<PRE>
N       (local input / 本地输入)                 const int
        On entry, N  specifies the number of columns of the matrix A.
        N must be at least zero.
</PRE>
<PRE>
ALPHA   (local input / 本地输入)                 const double
        On entry, ALPHA specifies the scalar alpha.   When  ALPHA  is
        supplied as zero then  A and X  need not be set on input.
</PRE>
<PRE>
A       (local input / 本地输入)                 const double *
        On entry,  A  points  to an array of size equal to or greater
        than LDA * n.  Before  entry, the leading m by n part  of the
        array  A  must contain the matrix coefficients.
</PRE>
<PRE>
LDA     (local input / 本地输入)                 const int
        On entry,  LDA  specifies  the  leading  dimension  of  A  as
        declared  in  the  calling  (sub) program.  LDA  must  be  at
        least MAX(1,m).
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
BETA    (local input / 本地输入)                 const double
        On entry, BETA  specifies the scalar beta.    When  ALPHA  is
        supplied as zero then  Y  need not be set on input.
</PRE>
<PRE>
Y       (local input/output / 本地输入/输出)          double *
        On entry,  Y  is an incremented array of dimension  at  least
        ( 1 + ( n - 1 ) * abs( INCY ) )  that  contains the vector y.
        Before entry with BETA non-zero, the incremented array Y must
        contain the vector  y.  On exit,  Y  is  overwritten  by  the
        updated vector y.
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
   double a[2*2], x[2], y[2];
   a[0] = 1.0; a[1] = 2.0; a[2] = 3.0; a[3] = 3.0;
   x[0] = 2.0; x[1] = 1.0; y[2] = 1.0; y[3] = 2.0;
   HPL_dgemv( HplColumnMajor, HplNoTrans, 2, 2, 2.0,
              a, 2, x, 1, -1.0, y, 1 );
   printf("y=[%f,%f]\n", y[0], y[1]);
   exit(0); return(0);
}
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_dger.md">HPL_dger</A>,
<A HREF="HPL_dtrsv.md">HPL_dtrsv</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
