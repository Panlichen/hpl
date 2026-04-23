<!-- Converted from HPL_dlaprnt HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_dlaprnt</B> Print the matrix A.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_dlaprnt(</CODE>
<CODE>const int</CODE>
<CODE>M</CODE>,
<CODE>const int</CODE>
<CODE>N</CODE>,
<CODE>double *</CODE>
<CODE>A</CODE>,
<CODE>const int</CODE>
<CODE>IA</CODE>,
<CODE>const int</CODE>
<CODE>JA</CODE>,
<CODE>const int</CODE>
<CODE>LDA</CODE>,
<CODE>const char *</CODE>
<CODE>CMATNM</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_dlaprnt</B>
prints to standard error an M-by-N matrix A.

<H1>Arguments / 参数</H1>
<PRE>
M       (local input / 本地输入)                 const int
        On entry,  M  specifies the number of rows of A. M must be at
        least zero.
</PRE>
<PRE>
N       (local input / 本地输入)                 const int
        On entry,  N  specifies the number of columns of A. N must be
        at least zero.
</PRE>
<PRE>
A       (local input / 本地输入)                 double *
        On entry, A  points to an array of dimension (LDA,N).
</PRE>
<PRE>
IA      (local input / 本地输入)                 const int
        On entry, IA specifies the starting row index to be printed.
</PRE>
<PRE>
JA      (local input / 本地输入)                 const int
        On entry,  JA  specifies  the  starting  column index  to be
        printed.
</PRE>
<PRE>
LDA     (local input / 本地输入)                 const int
        On entry, LDA specifies the leading dimension of the array A.
        LDA must be at least max(1,M).
</PRE>
<PRE>
CMATNM  (local input / 本地输入)                 const char *
        On entry, CMATNM is the name of the matrix to be printed.
</PRE>

<H1>Example</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<PRE>
int main(int argc, char *argv[])
{
   double a[2*2];
   a[0] = 1.0; a[1] = 3.0; a[2] = 2.0; a[3] = 4.0;
   HPL_dlaprnt( 2, 2, a, 0, 0, 2, "A" );
   exit(0); return(0);
}
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_fprintf.md">HPL_fprintf</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
