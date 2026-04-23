<!-- Converted from HPL Errata-Bugs / HPL 勘误-错误 -->

<H2>HPL Errata - Bugs / HPL 勘误 - 错误</H2>
<H3>Issues fixed in Version 2.1, October 26th, 2012 / 版本 2.1 中修复的问题，2012年10月26日</H3>
The output now reports exact time stamps before and after the execution of the solver function pdgesv() was run. This could allow for accurate accounting of running time for data center management   purposes.    For   example  as  reporting  power consumption.  This  is  important  for  the Green500 project.<BR><BR>
输出现在报告求解器函数 pdgesv() 运行前后的精确时间戳。这可以允许为数据中心管理目的进行准确的运行时间计算。例如，报告功耗。这对 Green500 项目很重要。<BR><BR>
Fixed an out-of-bounds access to arrays  in the HPL_spreadN() and  HPL_spreadT()  functions.  This  may cause  segmentation fault signals. It was reported by Stephen Whalen from Cray.<BR><BR>
修复了 HPL_spreadN() 和 HPL_spreadT() 函数中数组的越界访问。这可能导致段错误信号。由 Cray 的 Stephen Whalen 报告。<BR><BR>
<H3>Issues fixed in Version 2.0, September 10th, 2008 / 版本 2.0 中修复的问题，2008年9月10日</H3>
Gregory Bauer  found  a  problem  size  corresponding  to the periodicity of the pseudo-random matrix generator used in the HPL  timing  program. This causes  the  LU  factorization  to detect the singularity of the input matrix as it should have.<BR><BR>
Gregory Bauer 发现了一个与 HPL 计时程序中使用的伪随机矩阵生成器的周期性相对应的问题规模。这导致 LU 分解检测到输入矩阵的奇异性，正如它应该做的那样。<BR><BR>
A problem size of 2^17 = 131072 causes columns 14 modulo 2^14 (i.e. 16384)  (starting from 0)  to be bitwise identical on a homogeneous platform.  Every problem size  being a power of 2 and larger than  2^15  will  feature a similar problem if one searches far enough in the columns of the square input matrix.<BR><BR>
问题规模 2^17 = 131072 导致列 14 模 2^14（即 16384）（从 0 开始）在均匀平台上按位相同。每个为 2 的幂且大于 2^15 的问题规模，如果在方阵输入矩阵的列中搜索得足够远，都会出现类似的问题。<BR><BR>
The pseudo-random  generator  uses  the  linear  congruential algorithm:  X(n+1) = (a * X(n) + c) mod m as described in the Art of Computer  Programming, Knuth 1973,  Vol. 2. In the HPL case, m is set to 2^31.<BR><BR>
伪随机生成器使用线性同余算法：X(n+1) = (a * X(n) + c) mod m，如《计算机程序设计艺术》，Knuth 1973，第 2 卷中所述。在 HPL 的情况下，m 设置为 2^31。<BR><BR>
It is very important  to realize that this issue is a problem of  the  testing  part  of the  HPL software.  The  numerical properties  of the  algorithms  used in the factorization and the solve should not be questioned because of this.  In fact, this is just the opposite: the factorization demonstrated the weakness of the testing part of the software by detecting the singularity of the input matrix.<BR><BR>
非常重要的是要认识到，这个问题是 HPL 软件测试部分的问题。不应因此质疑分解和求解中使用的算法的数值特性。事实上，恰恰相反：分解通过检测输入矩阵的奇异性来展示了软件测试部分的弱点。<BR><BR>
This issue of  the testing program  is not easy to fix.  This pseudo-random  generator  has  very useful properties despite this.  It is  thus currently recommended to HPL users willing to test matrices of size larger than  2^15  to  not use power twos.<BR><BR>
测试程序的这个问题不容易修复。尽管如此，这个伪随机生成器具有非常有用的属性。因此，目前建议愿意测试大于 2^15 的大规模矩阵的 HPL 用户不要使用 2 的幂。<BR><BR>
This  issue  has  been fixed by  changing  the  pseudo-random matrix  generator.   Now the  periodicity of the generator is 2^64.<BR><BR>
此问题已通过更改伪随机矩阵生成器修复。现在生成器的周期为 2^64。<BR><BR>
<H3>Issues fixed in Version 1.0b, December 15th, 2004 / 版本 1.0b 中修复的问题，2004年12月15日</H3>
When the matrix size is such that one needs  more  than 16 GB per  MPI  rank,  the  intermediate  calculation  (mat.ld+1) * mat.nq in  HPL_pdtest.c  ends up  overflowing  because  it is done using  32-bit arithmetic.   This issue has been fixed by typecasting to size_t; Thanks to John Baron.<BR><BR>
当矩阵大小使得每个 MPI 排名需要超过 16 GB 时，HPL_pdtest.c 中的中间计算 (mat.ld+1) * mat.nq 会溢出，因为它是使用 32 位算术完成的。此问题已通过类型转换为 size_t 修复；感谢 John Baron。<BR><BR>
<H3>Issues fixed in Version 1.0a, January 20th, 2004 / 版本 1.0a 中修复的问题，2004年1月20日</H3>
The  MPI  process  grid numbering scheme defaults now to row-major ordering. This option can now be selected at run time.<BR><BR>
MPI 进程网格编号方案现在默认为行主序。此选项现在可以在运行时选择。<BR><BR>
The  inlined  assembly  timer  routine  that  was causing the compilation to fail when using gcc version  3.3 and above has been removed from the package.<BR><BR>
导致使用 gcc 3.3 及以上版本编译失败的内联汇编计时器例程已从软件包中移除。<BR><BR>
Various building problems on the T3E have been fixed;  Thanks to Edward Anderson.<BR><BR>
修复了 T3E 上的各种构建问题；感谢 Edward Anderson。<BR><BR>
<H3>Issues fixed in Version 1.0, September 27th, 2000 / 版本 1.0 中修复的问题，2000年9月27日</H3>
Due to a  couple errors  spotted in the  VSIPL  port  of  the software,  the  distribution  contained  in  the tar file of September 9th, 2000 had been updated on September 27th,  2000 with a corrected  distribution.  <STRONG>These  problems were not affecting in any way possible the  BLAS  version  of  the software.</STRONG>  If you are using  the  VSIPL port of HPL, and  want  to  make  sure you are  indeed  using  the latest corrected version, please  check  the  date  contained in the file HPL.build.log contained in the main directory.<BR><BR>
由于在软件的 VSIPL 移植中发现了几个错误，2000年9月9日的 tar 文件中包含的发行版已于2000年9月27日更新为修正后的发行版。<strong>这些问题不会以任何方式影响软件的 BLAS 版本。</strong>如果您正在使用 HPL 的 VSIPL 移植，并想确保您确实在使用最新的修正版本，请检查主目录中包含的 HPL.build.log 文件中的日期。<BR><BR>
<HR NOSHADE>
<CENTER>
<A HREF = "index.md">            [Home / 首页]</A>
<A HREF = "copyright.md">        [Copyright and Licensing Terms / 版权和许可条款]</A>
<A HREF = "algorithm.md">        [Algorithm / 算法]</A>
<A HREF = "scalability.md">      [Scalability / 可扩展性]</A>
<A HREF = "results.md">          [Performance Results / 性能结果]</A>
<A HREF = "documentation.md">    [Documentation / 文档]</A>
<A HREF = "software.md">         [Software / 软件]</A>
<A HREF = "faqs.md">             [FAQs / 常见问题]</A>
<A HREF = "tuning.md">           [Tuning / 调优]</A>
<A HREF = "errata.md">           [Errata-Bugs / 勘误-错误]</A>
<A HREF = "references.md">       [References / 参考文献]</A>
<A HREF = "links.md">            [Related Links / 相关链接]</A><BR>
</CENTER>
<HR NOSHADE>
