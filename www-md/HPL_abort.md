<!-- Converted from HPL_abort HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_abort</B> halts execution.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_abort(</CODE>
<CODE>int</CODE>
<CODE>LINE</CODE>,
<CODE>const char *</CODE>
<CODE>SRNAME</CODE>,
<CODE>const char *</CODE>
<CODE>FORM</CODE>,
<CODE>...</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_abort</B>
displays an error message on stderr and halts execution.

<H1>Arguments / 参数</H1>
<PRE>
LINE    (local input / 本地输入)                 int
        On entry,  LINE  specifies the line  number in the file where
        the  error  has  occured.  When  LINE  is not a positive line
        number, it is ignored.
</PRE>
<PRE>
SRNAME  (local input / 本地输入)                 const char *
        On entry, SRNAME  should  be the name of the routine  calling
        this error handler.
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
   HPL_abort( __LINE__, __FILE__, "Halt.\n" );
   exit(0); return(0);
}
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_fprintf.md">HPL_fprintf</A>,
<A HREF="HPL_warn.md">HPL_warn</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
