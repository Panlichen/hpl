/* 
 * -- High Performance Computing Linpack Benchmark (HPL)                
 *    HPL - 2.3 - December 2, 2018                          
 *    Antoine P. Petitet                                                
 *    University of Tennessee, Knoxville                                
 *    Innovative Computing Laboratory                                 
 *    (C) Copyright 2000-2008 All Rights Reserved                       
 *                                                                      
 * -- Copyright notice and Licensing terms:                             
 *                                                                      
 * Redistribution  and  use in  source and binary forms, with or without
 * modification, are  permitted provided  that the following  conditions
 * are met:                                                             
 *                                                                      
 * 1. Redistributions  of  source  code  must retain the above copyright
 * notice, this list of conditions and the following disclaimer.        
 *                                                                      
 * 2. Redistributions in binary form must reproduce  the above copyright
 * notice, this list of conditions,  and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. 
 *                                                                      
 * 3. All  advertising  materials  mentioning  features  or  use of this
 * software must display the following acknowledgement:                 
 * This  product  includes  software  developed  at  the  University  of
 * Tennessee, Knoxville, Innovative Computing Laboratory.             
 *                                                                      
 * 4. The name of the  University,  the name of the  Laboratory,  or the
 * names  of  its  contributors  may  not  be used to endorse or promote
 * products  derived   from   this  software  without  specific  written
 * permission.                                                          
 *                                                                      
 * -- Disclaimer:                                                       
 *                                                                      
 * THIS  SOFTWARE  IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY
 * OR  CONTRIBUTORS  BE  LIABLE FOR ANY  DIRECT,  INDIRECT,  INCIDENTAL,
 * SPECIAL,  EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES  (INCLUDING,  BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA OR PROFITS; OR BUSINESS INTERRUPTION)  HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT,  STRICT LIABILITY,  OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 * ---------------------------------------------------------------------
 */ 
/*
 * Include files
 */
#include "hpl.h"

#ifdef STDC_HEADERS
double HPL_pdlamch
(
   MPI_Comm                         COMM,
   const HPL_T_MACH                 CMACH
)
#else
double HPL_pdlamch
( COMM, CMACH )
   MPI_Comm                         COMM;
   const HPL_T_MACH                 CMACH;
#endif
{
/* 
 * Purpose
 * =======
 *
 * HPL_pdlamch determines  machine-specific  arithmetic  constants  such  as
 * the relative machine precision (eps),  the safe minimum(sfmin) such that
 * 1/sfmin does not overflow, the base of the machine (base), the precision
 * (prec),  the  number  of  (base)  digits in the  mantissa  (t),  whether
 * rounding occurs in addition (rnd = 1.0 and 0.0 otherwise),  the  minimum
 * exponent before  (gradual)  underflow (emin),  the  underflow  threshold
 * (rmin)- base**(emin-1), the largest exponent before overflow (emax), the
 * overflow threshold (rmax)  - (base**emax)*(1-eps).
 * HPL_pdlamch 用于获取与机器浮点算术相关的常量，例如相对机器精度
 * (eps)、安全最小值 (sfmin，满足 1/sfmin 不会上溢)、机器基数 (base)、
 * 精度 (prec)、尾数位数 (t)、加法是否发生舍入 (rnd，发生则为 1.0，
 * 否则为 0.0)、发生（渐进）下溢前的最小指数 (emin)、下溢阈值
 * (rmin = base**(emin-1))、发生上溢前的最大指数 (emax)，以及上溢阈值
 * (rmax = (base**emax)*(1-eps))。
 *
 * In HPL this routine is not merely a local LAPACK wrapper: it first asks
 * the local process for its floating-point constant via HPL_dlamch, then
 * uses MPI-level collective reduction so that every rank works with a value
 * that is safe for the whole communicator.
 * 在 HPL 中，这个例程不仅仅是对本地 LAPACK `dlamch` 的简单封装：它先
 * 在当前进程上通过 HPL_dlamch 取得本地浮点常量，再借助 MPI 层面的集体
 * 归约，把它变成对整个通信域都“安全一致”的全局参数。
 *
 * Arguments
 * =========
 *
 * COMM    (global/local input)          MPI_Comm
 *         The MPI communicator identifying the process collection.
 * COMM    （全局/局部输入）             MPI_Comm
 *         标识参与本次机器参数统一的进程集合。
 *
 * CMACH   (global input)                const HPL_T_MACH
 *         Specifies the value to be returned by HPL_pdlamch            
 *            = HPL_MACH_EPS,   HPL_pdlamch := eps (default)            
 *            = HPL_MACH_SFMIN, HPL_pdlamch := sfmin                    
 *            = HPL_MACH_BASE,  HPL_pdlamch := base                     
 *            = HPL_MACH_PREC,  HPL_pdlamch := eps*base                 
 *            = HPL_MACH_MLEN,  HPL_pdlamch := t                        
 *            = HPL_MACH_RND,   HPL_pdlamch := rnd                      
 *            = HPL_MACH_EMIN,  HPL_pdlamch := emin                     
 *            = HPL_MACH_RMIN,  HPL_pdlamch := rmin                     
 *            = HPL_MACH_EMAX,  HPL_pdlamch := emax                     
 *            = HPL_MACH_RMAX,  HPL_pdlamch := rmax                     
 *          
 *         where                                                        
 *          
 *            eps   = relative machine precision,                       
 *            sfmin = safe minimum,                                     
 *            base  = base of the machine,                              
 *            prec  = eps*base,                                         
 *            t     = number of digits in the mantissa,                 
 *            rnd   = 1.0 if rounding occurs in addition,               
 *            emin  = minimum exponent before underflow,                
 *            rmin  = underflow threshold,                              
 *            emax  = largest exponent before overflow,                 
 *            rmax  = overflow threshold.
 * CMACH   （全局输入）                  const HPL_T_MACH
 *         指定 HPL_pdlamch 要返回哪一个机器参数：
 *            = HPL_MACH_EPS,   返回 eps（默认）
 *            = HPL_MACH_SFMIN, 返回 sfmin
 *            = HPL_MACH_BASE,  返回 base
 *            = HPL_MACH_PREC,  返回 eps*base
 *            = HPL_MACH_MLEN,  返回 t
 *            = HPL_MACH_RND,   返回 rnd
 *            = HPL_MACH_EMIN,  返回 emin
 *            = HPL_MACH_RMIN,  返回 rmin
 *            = HPL_MACH_EMAX,  返回 emax
 *            = HPL_MACH_RMAX,  返回 rmax
 *
 *         其中：
 *            eps   = 相对机器精度
 *            sfmin = 安全最小值
 *            base  = 浮点表示的基数
 *            prec  = eps*base
 *            t     = 尾数位数
 *            rnd   = 加法是否舍入
 *            emin  = 下溢前的最小指数
 *            rmin  = 下溢阈值
 *            emax  = 上溢前的最大指数
 *            rmax  = 上溢阈值
 *
 * ---------------------------------------------------------------------
 */ 
/*
 * .. Local Variables ..
 *
 * param :
 *   local machine constant returned by HPL_dlamch first, then possibly
 *   overwritten by a communicator-wide reduction so that all ranks use the
 *   same globally safe value. The final param is returned to the caller.
 * param：
 *   先保存当前进程通过 HPL_dlamch 得到的本地机器参数；随后如果需要，会
 *   被一次通信域范围的归约结果覆盖，从而保证所有 rank 使用同一个、对整
 *   个通信器都安全的值。函数最终返回的就是这个 param。
 */
   double                     param;
/* ..
 * .. Executable Statements ..
 */
/*
 * Step 1: query the local floating-point environment.
 * 第一步：读取当前进程本地浮点环境中的机器参数。
 *
 * HPL_dlamch 与 LAPACK dlamch 的语义一致，它反映的是“本进程当前编译/
 * 运行环境下的双精度特性”。如果所有进程的硬件与编译选项完全一致，那么
 * 大家拿到的值通常相同；但 HPL 仍然会在部分参数上做一次全局统一，以保
 * 证后续残差阈值和安全范围判断在整个并行作业里一致。
 */
   param = HPL_dlamch( CMACH );

/*
 * Step 2: promote selected local values to a communicator-wide safe value.
 * 第二步：把部分本地机器参数提升为通信域范围内统一且安全的全局值。
 *
 * Why `epsil` uses HPL_max:
 * 为什么 epsil 要使用 HPL_max：
 *
 * - `eps` 表示“1.0 与下一个可表示浮点数之间的相对间距”，它越大，说明
 *   该进程的有效精度越差。
 * - HPL 的残差判据会用 eps 作为误差归一化尺度。如果不同进程的 eps 存在
 *   差异，而我们又想让整个并行作业使用同一个统一阈值，那么必须选择那个
 *   “最保守”的 eps，也就是所有进程里最大的 eps。
 * - 因此这里对 HPL_MACH_EPS 做 HPL_all_reduce(..., HPL_max, COMM)，
 *   其含义是：取整个通信域中最差精度对应的相对机器精度，确保后续数值误
 *   差评估不会对任何一个进程过于乐观。
 *
 * The same conservative logic applies to:
 * 相同的“保守取值”逻辑也适用于：
 * - sfmin: choose the largest safe minimum so that 1/sfmin stays safe for
 *   every process;
 *   sfmin：取最大的安全最小值，保证对所有进程来说 1/sfmin 都仍然安全；
 * - emin/rmin: choose the largest underflow-related thresholds so the global
 *   code does not assume a smaller representable range than some rank has.
 *   emin/rmin：取与下溢相关的较大阈值，避免全局代码假设某些进程拥有比
 *   实际更宽的可表示下界。
 *
 * For emax/rmax the direction is reversed:
 * 对 emax/rmax 则反过来处理：
 * - a smaller maximum exponent or overflow threshold is the safer global
 *   choice, because it represents the weakest overflow bound across ranks.
 * - 因为最大指数和上溢阈值越小，表示越容易上溢；对全局而言，取所有进
 *   程中最小的 emax/rmax，才是对整个通信域最安全、最保守的统一上界。
 *
 * base/prec/t/rnd are left unchanged because HPL expects these fundamental
 * format traits to match across ranks in a normal homogeneous run.
 * base/prec/t/rnd 则保持本地值不变，因为在正常的同构集群运行场景下，
 * HPL 默认这些基础浮点格式属性在各个进程上应当一致。
 */
   switch( CMACH )
   {
      case HPL_MACH_EPS   :
      case HPL_MACH_SFMIN :
      case HPL_MACH_EMIN  :
      case HPL_MACH_RMIN  :
/*
 * Collective communication: all-reduce with the max operator.
 * 集合通信：使用 max 算子的 all-reduce。
 *
 * This is a true collective semantic in the HPL wrapper layer: every process
 * in COMM contributes its local `param`, and everyone receives the same
 * reduced result.
 * 这里体现的是典型的集合通信语义：COMM 中的每个进程都贡献自己的本地
 * `param`，归约结束后每个进程都拿到同一个结果。
 */
         (void) HPL_all_reduce( (void *)(&param), 1, HPL_DOUBLE,
                                HPL_max, COMM );
         break;
      case HPL_MACH_EMAX  :
      case HPL_MACH_RMAX  :
/*
 * Collective communication: all-reduce with the min operator.
 * 集合通信：使用 min 算子的 all-reduce。
 *
 * For overflow-related limits we want the most restrictive globally safe
 * value, hence the minimum.
 * 对与上溢相关的限制量，需要取全局最严格、最保守的安全值，因此这里用
 * 最小值归约。
 */
         (void) HPL_all_reduce( (void *)(&param), 1, HPL_DOUBLE,
                                HPL_min, COMM );
         break;
      default             :
         break;
   } 

   return( param );
/*
 * End of HPL_pdlamch
 * HPL_pdlamch 结束。
 */
}
