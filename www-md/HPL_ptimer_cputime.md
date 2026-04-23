<!-- Converted from HPL_ptimer_cputime HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_ptimer_cputime</B> Return the CPU time.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>double</CODE>
<CODE>HPL_ptimer_cputime();</CODE>

<H1>Description / 描述</H1>
<B>HPL_ptimer_cputime</B>
returns the cpu time. If HPL_USE_CLOCK is defined,
the  clock() function is used to return an approximation of processor
time used by the program.  The value returned is the CPU time used so
far as a clock_t;  to get the number of seconds used,  the result  is
divided by  CLOCKS_PER_SEC.  This function is part of the  ANSI/ISO C
standard library.  If  HPL_USE_TIMES is defined, the times() function
is used instead.  This  function  returns  the current process times.
times() returns the number of clock ticks that have elapsed since the
system has been up.  Otherwise and by default,  the  standard library
function getrusage() is used.

<H1>See Also / 另见</H1>
<A HREF="HPL_ptimer_walltime.md">HPL_ptimer_walltime</A>,
<A HREF="HPL_ptimer.md">HPL_ptimer</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
