<!-- Converted from HPL_fprintf HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_fprintf</B> fprintf + fflush wrapper.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_fprintf(</CODE>
<CODE>FILE *</CODE>
<CODE>STREAM</CODE>,
<CODE>const char *</CODE>
<CODE>FORM</CODE>,
<CODE>...</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_fprintf</B>
is a wrapper around fprintf flushing the output stream.

<H1>Arguments / 参数</H1>
<PRE>
STREAM  (local input / 本地输入)                 FILE *
        On entry, STREAM specifies the output stream.
</PRE>
<PRE>
FORM    (local input / 本地输入)                 const char *
        On entry, FORM specifies the format, i.e., how the subsequent
        arguments are converted for output.
</PRE>
<PRE>
        (local input / 本地输入)                 ...
        On entry,  ...  is the list of arguments to be printed within
        the format string.
</PRE>

<H1>Example</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<PRE>
int main(int argc, char *argv[])
{
   HPL_fprintf( stdout, "Hello World.\n" );
   exit(0); return(0);
}
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_abort.md">HPL_abort</A>,
<A HREF="HPL_warn.md">HPL_warn</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
