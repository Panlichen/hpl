<!-- Converted from HPL_dlange HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_dlange</B> Compute ||A||.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>double</CODE>
<CODE>HPL_dlange(</CODE>
<CODE>const HPL_T_NORM</CODE>
<CODE>NORM</CODE>,
<CODE>const int</CODE>
<CODE>M</CODE>,
<CODE>const int</CODE>
<CODE>N</CODE>,
<CODE>const double *</CODE>
<CODE>A</CODE>,
<CODE>const int</CODE>
<CODE>LDA</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_dlange</B>
returns  the value of the one norm,  or the infinity norm,
or the element of largest absolute value of a matrix A:              
 
   max(abs(A(i,j))) when NORM = HPL_NORM_A,                          
   norm1(A),        when NORM = HPL_NORM_1,                          
   normI(A),        when NORM = HPL_NORM_I,                          
 
where norm1 denotes the one norm of a matrix (maximum column sum) and
normI denotes  the infinity norm of a matrix (maximum row sum).  Note
that max(abs(A(i,j))) is not a matrix norm.

<H1>Arguments / 参数</H1>
<PRE>
NORM    (local input / 本地输入)                 const HPL_T_NORM
        On entry,  NORM  specifies  the  value to be returned by this
        function as described above.
</PRE>
<PRE>
M       (local input / 本地输入)                 const int
        On entry,  M  specifies  the number  of rows of the matrix A.
        M must be at least zero.
</PRE>
<PRE>
N       (local input / 本地输入)                 const int
        On entry,  N specifies the number of columns of the matrix A.
        N must be at least zero.
</PRE>
<PRE>
A       (local input / 本地输入)                 const double *
        On entry,  A  points to an  array of dimension  (LDA,N), that
        contains the matrix A.
</PRE>
<PRE>
LDA     (local input / 本地输入)                 const int
        On entry, LDA specifies the leading dimension of the array A.
        LDA must be at least max(1,M).
</PRE>

<H1>Example</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<PRE>
int main(int argc, char *argv[])
{
   double a[2*2];
   a[0] = 1.0; a[1] = 3.0; a[2] = 2.0; a[3] = 4.0;
   norm = HPL_dlange( HPL_NORM_I, 2, 2, a, 2 );
   printf("norm=%f\n", norm);
   exit(0); return(0);
}
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_dlaprnt.md">HPL_dlaprnt</A>,
<A HREF="HPL_fprintf.md">HPL_fprintf</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
