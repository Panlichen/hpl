<!-- Converted from HPL_ptimer HPL 2.3 Library Functions December 2, 2018 -->

<H1>Name / 名称</H1>
<B>HPL_ptimer</B> Timer facility.

<H1>Synopsis / 概要</H1>
<CODE>#include "hpl.h"</CODE><BR><BR>
<CODE>void</CODE>
<CODE>HPL_ptimer(</CODE>
<CODE>const int</CODE>
<CODE>I</CODE>
<CODE>);</CODE>

<H1>Description / 描述</H1>
<B>HPL_ptimer</B>
provides a  "stopwatch"  functionality  cpu/wall  timer in
seconds.  Up to  64  separate timers can be functioning at once.  The
first call starts the timer,  and the second stops it.  This  routine
can be disenabled  by calling HPL_ptimer_disable(),  so that calls to
the timer are ignored.  This feature can be used to make sure certain
sections of code do not affect timings,  even  if  they call routines
which have HPL_ptimer calls in them. HPL_ptimer_enable()  will enable
the  timer  functionality.  One  can retrieve  the current value of a
timer by calling
 
t0 = HPL_ptimer_inquire( HPL_WALL_TIME | HPL_CPU_TIME, I )
 
where  I  is the timer index in  [0..64).  To  inititialize the timer
functionality, one must have called HPL_ptimer_boot() prior to any of
the functions mentioned above.

<H1>Arguments / 参数</H1>
<PRE>
I       (global input / 全局输入)                const int
        On entry, I specifies the timer to stop/start.
</PRE>

<H1>See Also / 另见</H1>
<A HREF="HPL_ptimer_cputime.md">HPL_ptimer_cputime</A>,
<A HREF="HPL_ptimer_walltime.md">HPL_ptimer_walltime</A>.

<HR NOSHADE><P><EM>See the original English page for complete documentation. / 完整文档请参见原始英文页面。</EM></P>
