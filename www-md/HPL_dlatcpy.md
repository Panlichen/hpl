<!-- Converted from HPL_dlatcpy HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_dlatcpy</B> B := A^T

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_dlatcpy(</CODE>
<CODE>const int</CODE>
<CODE>M</CODE>,
<CODE>const int</CODE>
<CODE>N</CODE>,
<CODE>const double *</CODE>
<CODE>A</CODE>,
<CODE>const int</CODE>
<CODE>LDA</CODE>,
<CODE>double *</CODE>
<CODE>B</CODE>,
<CODE>const int</CODE>
<CODE>LDB</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_dlatcpy</B>
copies the transpose of an array A into an array B.

<H1>Arguments / 参数</H1>
<PRE>
M       (local input / 本地输入)                 const int
        On entry,  M specifies the number of  rows of the array B and
        the number of columns of A. M must be at least zero.
</PRE>
<PRE>
N       (local input / 本地输入)                 const int
        On entry,  N specifies the number of  rows of the array A and
        the number of columns of B. N must be at least zero.
</PRE>
<PRE>
A       (local input / 本地输入)                 const double *
        On entry, A points to an array of dimension (LDA,M).
</PRE>
<PRE>
LDA     (local input / 本地输入)                 const int
        On entry, LDA specifies the leading dimension of the array A.
        LDA must be at least MAX(1,N).
</PRE>
<PRE>
B       (local output / 本地输出)                double *
        On entry, B points to an array of dimension (LDB,N). On exit,
        B is overwritten with the transpose of A.
</PRE>
<PRE>
LDB     (local input / 本地输入)                 const int
        On entry, LDB specifies the leading dimension of the array B.
        LDB must be at least MAX(1,M).
</PRE>

<H1>Example</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<PRE>
int main(int argc, char *argv[])
{
   double a[2*2], b[2*2];
   a[0] = 1.0; a[1] = 3.0; a[2] = 2.0; a[3] = 4.0;
   HPL_dlacpy( 2, 2, a, 2, b, 2 );
   printf("  [%f,%f]\n", b[0], b[2]);
   printf("b=[%f,%f]\n", b[1], b[3]);
   exit(0); return(0);
}
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_dlacpy.md">HPL_dlacpy</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
