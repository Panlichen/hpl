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
void HPL_pdtest
(
   HPL_T_test *                     TEST,
   HPL_T_grid *                     GRID,
   HPL_T_palg *                     ALGO,
   const int                        N,
   const int                        NB
)
#else
void HPL_pdtest
( TEST, GRID, ALGO, N, NB )
   HPL_T_test *                     TEST;
   HPL_T_grid *                     GRID;
   HPL_T_palg *                     ALGO;
   const int                        N;
   const int                        NB;
#endif
{
/* 
 * Purpose
 * =======
 *
 * HPL_pdtest performs  one  test  given a set of parameters such as the
 * process grid, the  problem size, the distribution blocking factor ...
 * This function generates  the data, calls  and times the linear system
 * solver,  checks  the  accuracy  of the  obtained vector solution  and
 * writes this information to the file pointed to by TEST->outfp.
 * HPL_pdtest 在给定一组参数（例如进程网格、问题规模、分布块大小等）的
 * 情况下执行一次完整测试。该函数负责生成测试数据，调用并计时线性方程组
 * 求解器，检查得到的向量解是否正确，并将结果写入 TEST->outfp 指向的输
 * 出文件。
 *
 * Arguments
 * =========
 *
 * TEST    (global input)                HPL_T_test *
 *         On entry,  TEST  points  to a testing data structure:  outfp
 *         specifies the output file where the results will be printed.
 *         It is only defined and used by the process  0  of the  grid.
 *         thrsh  specifies  the  threshhold value  for the test ratio.
 *         Concretely, a test is declared "PASSED"  if and only if the
 *         following inequality is satisfied:
 *         ||Ax-b||_oo / ( epsil *
 *                         ( || x ||_oo * || A ||_oo + || b ||_oo ) *
 *                          N )  < thrsh.
 *         epsil  is the  relative machine precision of the distributed
 *         computer. Finally the test counters, kfail, kpass, kskip and
 *         ktest are updated as follows:  if the test passes,  kpass is
 *         incremented by one;  if the test fails, kfail is incremented
 *         by one; if the test is skipped, kskip is incremented by one.
 *         ktest is left unchanged.
 * TEST    （全局输入）                  HPL_T_test *
 *         入口时，TEST 指向测试控制结构。outfp 指定输出结果写入的文件，
 *         它只在网格中的 (0,0) 进程上有效并被实际使用。thrsh 给出残差比
 *         例阈值；只有当下面不等式成立时，测试才会被判定为 “PASSED”：
 *         ||Ax-b||_oo / ( epsil *
 *                         ( || x ||_oo * || A ||_oo + || b ||_oo ) *
 *                          N )  < thrsh.
 *         epsil 表示分布式机器的相对机器精度。最后，测试计数器按如下规则
 *         更新：通过则 kpass 加一；失败则 kfail 加一；跳过则 kskip 加一；
 *         ktest 在本函数中不修改。
 *
 * GRID    (local input)                 HPL_T_grid *
 *         On entry,  GRID  points  to the data structure containing the
 *         process grid information.
 * GRID    （局部输入）                  HPL_T_grid *
 *         入口时，GRID 指向保存当前二维进程网格信息的数据结构。
 *
 * ALGO    (global input)                HPL_T_palg *
 *         On entry,  ALGO  points to  the data structure containing the
 *         algorithmic parameters to be used for this test.
 * ALGO    （全局输入）                  HPL_T_palg *
 *         入口时，ALGO 指向本次测试要使用的算法参数集合。
 *
 * N       (global input)                const int
 *         On entry,  N specifies the order of the coefficient matrix A.
 *         N must be at least zero.
 * N       （全局输入）                  const int
 *         入口时，N 给出系数矩阵 A 的阶数，必须不小于 0。
 *
 * NB      (global input)                const int
 *         On entry,  NB specifies the blocking factor used to partition
 *         and distribute the matrix A. NB must be larger than one.
 * NB      （全局输入）                  const int
 *         入口时，NB 指定矩阵 A 的分块与分布块大小，必须大于 1。
 *
 * ---------------------------------------------------------------------
 */ 
/*
 * .. Local Variables ..
 *
 * mat :
 *   distributed matrix descriptor for the local slice of [A | b] and the
 *   replicated/distributed solution storage mat.X.
 * mat：
 *   分布式矩阵描述符，管理本地持有的 [A | b] 片段以及求解结果 mat.X。
 *
 * wtime :
 *   combined wall-clock timing returned by HPL_ptimer_combine.
 * wtime：
 *   由 HPL_ptimer_combine 聚合出的总墙钟时间。
 *
 * info :
 *   temporary three-int buffer used for allocation-failure detection via
 *   HPL_all_reduce(HPL_max).
 * info：
 *   长度为 3 的临时整型缓冲区，用于通过 HPL_all_reduce(HPL_max)
 *   检查全局内存分配是否失败。
 *
 * Anorm1/AnormI/Xnorm1/XnormI/BnormI/resid0/resid1 :
 *   norms and residual metrics used for the final correctness check.
 * Anorm1/AnormI/Xnorm1/XnormI/BnormI/resid0/resid1：
 *   最终正确性校验所需的各种范数与残差指标。
 *
 * Bptr :
 *   local pointer to the right-hand side / residual column inside [A | b].
 * Bptr：
 *   指向 [A | b] 中右端项列（以及后续残差列）的本地指针。
 *
 * vptr :
 *   raw aligned allocation that backs mat.A and mat.X.
 * vptr：
 *   对齐分配得到的原始内存块，后续 mat.A 与 mat.X 都从它切分出来。
 *
 * first :
 *   static flag that prints the performance table header only once.
 * first：
 *   静态标志，确保性能表头只打印一次。
 *
 * ii/ip2 :
 *   temporaries used when constructing a legal leading dimension mat.ld.
 * ii/ip2：
 *   用于构造合法主维 mat.ld 的临时变量。
 *
 * myrow/mycol/nprow/npcol/nq :
 *   local grid coordinates and the local number of matrix columns.
 * myrow/mycol/nprow/npcol/nq：
 *   当前进程的网格坐标以及本地拥有的列数。
 *
 * ctop/cpfact/crfact :
 *   one-character codes emitted in the final result line to describe the
 *   chosen broadcast / pfact / rfact configuration.
 * ctop/cpfact/crfact：
 *   结果摘要行中的单字符编码，用来表示广播 / pfact / rfact 选择。
 *
 * HPL_w :
 *   detailed timing buckets gathered from HPL's internal timers; each slot
 *   is later printed to show where the solve spent time when detailed timing
 *   support is enabled.
 * HPL_w：
 *   HPL 内部细粒度计时器的聚合结果数组；在启用详细计时时，会用它输出
 *   rfact、pfact、update、ptrsv 等阶段的墙钟时间。
 *
 * Gflops :
 *   aggregate performance metric derived from the measured wall time and the
 *   standard HPL flop-count model; printed only by rank (0,0).
 * Gflops：
 *   根据墙钟时间和 HPL 标准浮点运算量模型计算出的整体性能指标，仅由
 *   (0,0) 号进程打印到结果表中。
 *
 * current_time_start/current_time_end :
 *   coarse wall-clock timestamps captured before and after HPL_pdgesv so that
 *   the output file records the human-readable execution window.
 * current_time_start/current_time_end：
 *   在调用 HPL_pdgesv 前后记录的粗粒度时间戳，用于在输出文件中打印人类
 *   可读的开始/结束时间。
 */
#ifdef HPL_DETAILED_TIMING
   double                     HPL_w[HPL_TIMING_N];
#endif
   HPL_T_pmat                 mat;  // 管理本地持有的 [A | b] 片段以及求解结果 mat.X
   double                     wtime[1];
   int                        info[3];
   double                     Anorm1, AnormI, Gflops, Xnorm1, XnormI,
                              BnormI, resid0, resid1;
   double                     * Bptr;
   void                       * vptr = NULL;
   static int                 first=1;
   int                        ii, ip2, mycol, myrow, npcol, nprow, nq;
   char                       ctop, cpfact, crfact;
   time_t                     current_time_start, current_time_end;
/* ..
 * .. Executable Statements ..
 */
   (void) HPL_grid_info( GRID, &nprow, &npcol, &myrow, &mycol );

   mat.n  = N; mat.nb = NB; mat.info = 0;
   mat.mp = HPL_numroc( N, NB, NB, myrow, 0, nprow );
   nq     = HPL_numroc( N, NB, NB, mycol, 0, npcol );
   mat.nq = nq + 1;
/*
 * Allocate matrix, right-hand-side, and vector solution x. [ A | b ] is
 * N by N+1.  One column is added in every process column for the solve.
 * The  result  however  is stored in a 1 x N vector replicated in every
 * process row. In every process, A is lda * (nq+1), x is 1 * nq and the
 * workspace is mp.
 * 分配矩阵、右端项以及解向量 x。[ A | b ] 的全局尺寸为 N x (N+1)，
 * 为了求解，矩阵右侧额外附带一列 b。最终解被存成一个 1 x N 的向量，
 * 并在每一条进程行中保留副本。对每个进程来说，A 的本地存储大小是
 * lda * (nq+1)，x 的本地长度是 nq，额外工作空间规模与 mp 对齐。
 *
 * Ensure that lda is a multiple of ALIGN and not a power of 2.
 * 保证 lda 既是 ALIGN 的整数倍，又避免取 2 的幂，以减轻潜在的缓存/
 * 存储体冲突问题。
 */
   /* local leading dimension */
   mat.ld = ( ( Mmax( 1, mat.mp ) - 1 ) / ALGO->align ) * ALGO->align;
   do
   {
      ii = ( mat.ld += ALGO->align ); ip2 = 1;
      while( ii > 1 ) { ii >>= 1; ip2 <<= 1; }
   }
   while( mat.ld == ip2 );
/*
 * Allocate dynamic memory
 * 分配动态内存。
 *
 * vptr 持有这次统一分配得到的连续原始内存，随后会从中切出 mat.A 与
 * mat.X。这里先用 HPL_all_reduce(..., HPL_max, GRID->all_comm) 汇总
 * 所有进程的分配状态；只要任意一个 rank 失败，整个测试就必须跳过，
 * 以避免后续分布式求解在部分进程缺失数据的情况下继续执行。
 */
   vptr = (void*)malloc( ( (size_t)(ALGO->align) + 
                           (size_t)(mat.ld+1) * (size_t)(mat.nq) ) *
                         sizeof(double) );
   info[0] = (vptr == NULL); info[1] = myrow; info[2] = mycol;
   (void) HPL_all_reduce( (void *)(info), 3, HPL_INT, HPL_max,
                          GRID->all_comm );
   if( info[0] != 0 )
   {
      if( ( myrow == 0 ) && ( mycol == 0 ) )
         HPL_pwarn( TEST->outfp, __LINE__, "HPL_pdtest",
                    "[%d,%d] %s", info[1], info[2],
                    "Memory allocation failed for A, x and b. Skip." );
      (TEST->kskip)++;
      /* some processes might have succeeded with allocation */
      /* 某些进程可能已经成功分配到内存，因此这里需要分别释放本地资源。 */
      if (vptr) free(vptr);
      return;
   }
/*
 * generate matrix and right-hand-side, [ A | b ] which is N by N+1.
 * 生成矩阵和右端项 [ A | b ]，其全局尺寸为 N x (N+1)。
 *
 * HPL_pdmatgen 会按照与正式求解相同的块循环分布方式填充本地子块，这样
 * 后续 HPL_pdgesv 能直接在分布式矩阵上工作，无需再做额外重排。
 */
   mat.A  = (double *)HPL_PTR( vptr,
                               ((size_t)(ALGO->align) * sizeof(double) ) );
   mat.X  = Mptr( mat.A, 0, mat.nq, mat.ld );
   HPL_pdmatgen( GRID, N, N+1, NB, mat.A, mat.ld, HPL_ISEED );
#ifdef HPL_CALL_VSIPL
   mat.block = vsip_blockbind_d( (vsip_scalar_d *)(mat.A),
                                 (vsip_length)(mat.ld * mat.nq),
                                 VSIP_MEM_NONE );
#endif
/*
 * Solve linear system.
 * 求解线性系统。
 *
 * HPL_barrier synchronizes the grid before timing so that the measured
 * interval covers the same distributed solve on every rank.  The barrier
 * itself is implemented inside HPL using the library broadcast wrapper.
 * HPL_barrier 会在计时前同步整个进程网格，确保所有 rank 测到的是同
 * 一次分布式求解区间。这个 barrier 本身也是 HPL 在库内部封装的通信。
 */
   HPL_ptimer_boot(); (void) HPL_barrier( GRID->all_comm );
   time( &current_time_start );
   HPL_ptimer( 0 );
   HPL_pdgesv( GRID, ALGO, &mat );
   HPL_ptimer( 0 );
   time( &current_time_end );
#ifdef HPL_CALL_VSIPL
   (void) vsip_blockrelease_d( mat.block, VSIP_TRUE ); 
   vsip_blockdestroy_d( mat.block );
#endif
/*
 * Gather max of all CPU and WALL clock timings and print timing results
 * 汇总所有进程的 CPU / 墙钟计时，并输出性能结果。
 *
 * HPL_ptimer_combine 这里取的是全局最大墙钟时间，因为并行求解的端到端
 * 完成时间由最慢进程决定；因此最终性能统计应该以“最慢 rank 的耗时”
 * 作为整个分布式求解的有效时间。
 */
   HPL_ptimer_combine( GRID->all_comm, HPL_AMAX_PTIME, HPL_WALL_PTIME,
                       1, 0, wtime );

   if( ( myrow == 0 ) && ( mycol == 0 ) )
   {
      if( first )
      {
         HPL_fprintf( TEST->outfp, "%s%s\n",
                      "========================================",
                      "========================================" );
         HPL_fprintf( TEST->outfp, "%s%s\n",
                      "T/V                N    NB     P     Q",
                      "               Time                 Gflops" );
         HPL_fprintf( TEST->outfp, "%s%s\n",
                      "----------------------------------------",
                      "----------------------------------------" );
         if( TEST->thrsh <= HPL_rzero ) first = 0;
      }
/*
 * 2/3 N^3 - 1/2 N^2 flops for LU factorization + 2 N^2 flops for solve.
 * Print WALL time
 * LU 分解的理论运算量约为 2/3 N^3 - 1/2 N^2，再加上线性方程求解阶段
 * 约 2 N^2 的计算量。这里使用墙钟时间来换算 GFLOPS。
 */
      Gflops = ( ( (double)(N) /   1.0e+9 ) * 
                 ( (double)(N) / wtime[0] ) ) * 
                 ( ( 2.0 / 3.0 ) * (double)(N) + ( 3.0 / 2.0 ) );

      cpfact = ( ( (HPL_T_FACT)(ALGO->pfact) == 
                   (HPL_T_FACT)(HPL_LEFT_LOOKING) ) ?  (char)('L') :
                 ( ( (HPL_T_FACT)(ALGO->pfact) == (HPL_T_FACT)(HPL_CROUT) ) ?
                   (char)('C') : (char)('R') ) );
      crfact = ( ( (HPL_T_FACT)(ALGO->rfact) == 
                   (HPL_T_FACT)(HPL_LEFT_LOOKING) ) ?  (char)('L') :
                 ( ( (HPL_T_FACT)(ALGO->rfact) == (HPL_T_FACT)(HPL_CROUT) ) ? 
                   (char)('C') : (char)('R') ) );

      if(      ALGO->btopo == HPL_1RING   ) ctop = '0';
      else if( ALGO->btopo == HPL_1RING_M ) ctop = '1';
      else if( ALGO->btopo == HPL_2RING   ) ctop = '2';
      else if( ALGO->btopo == HPL_2RING_M ) ctop = '3';
      else if( ALGO->btopo == HPL_BLONG   ) ctop = '4';
      else /* if( ALGO->btopo == HPL_BLONG_M ) */ ctop = '5';

      if( wtime[0] > HPL_rzero ) {
         HPL_fprintf( TEST->outfp,
             "W%c%1d%c%c%1d%c%1d%12d %5d %5d %5d %18.2f    %19.4e\n",
             ( GRID->order == HPL_ROW_MAJOR ? 'R' : 'C' ),
             ALGO->depth, ctop, crfact, ALGO->nbdiv, cpfact, ALGO->nbmin,
             N, NB, nprow, npcol, wtime[0], Gflops );
         HPL_fprintf( TEST->outfp,
             "HPL_pdgesv() start time %s\n", ctime( &current_time_start ) );
         HPL_fprintf( TEST->outfp,
             "HPL_pdgesv() end time   %s\n", ctime( &current_time_end ) );
      }
   }
#ifdef HPL_DETAILED_TIMING
   HPL_ptimer_combine( GRID->all_comm, HPL_AMAX_PTIME, HPL_WALL_PTIME,
                       HPL_TIMING_N, HPL_TIMING_BEG, HPL_w );
   if( ( myrow == 0 ) && ( mycol == 0 ) )
   {
      HPL_fprintf( TEST->outfp, "%s%s\n",
                   "--VVV--VVV--VVV--VVV--VVV--VVV--VVV--V",
                   "VV--VVV--VVV--VVV--VVV--VVV--VVV--VVV-" );
/*
 * Recursive panel factorization
 * 递归面板分解时间。
 */
      if( HPL_w[HPL_TIMING_RPFACT-HPL_TIMING_BEG] > HPL_rzero )
         HPL_fprintf( TEST->outfp,
                      "Max aggregated wall time rfact . . . : %18.2f\n",
                      HPL_w[HPL_TIMING_RPFACT-HPL_TIMING_BEG] );
/*
 * Panel factorization
 * 面板分解时间。
 */
      if( HPL_w[HPL_TIMING_PFACT-HPL_TIMING_BEG] > HPL_rzero )
         HPL_fprintf( TEST->outfp,
                      "+ Max aggregated wall time pfact . . : %18.2f\n",
                      HPL_w[HPL_TIMING_PFACT-HPL_TIMING_BEG] );
/*
 * Panel factorization (swap)
 * 面板分解中的交换时间。
 */
      if( HPL_w[HPL_TIMING_MXSWP-HPL_TIMING_BEG] > HPL_rzero )
         HPL_fprintf( TEST->outfp,
                      "+ Max aggregated wall time mxswp . . : %18.2f\n",
                      HPL_w[HPL_TIMING_MXSWP-HPL_TIMING_BEG] );
/*
 * Update
 * 尾随子矩阵更新时间。
 */
      if( HPL_w[HPL_TIMING_UPDATE-HPL_TIMING_BEG] > HPL_rzero )
         HPL_fprintf( TEST->outfp,
                      "Max aggregated wall time update  . . : %18.2f\n",
                      HPL_w[HPL_TIMING_UPDATE-HPL_TIMING_BEG] );
/*
 * Update (swap)
 * 更新阶段中的行交换时间。
 */
      if( HPL_w[HPL_TIMING_LASWP-HPL_TIMING_BEG] > HPL_rzero )
         HPL_fprintf( TEST->outfp,
                      "+ Max aggregated wall time laswp . . : %18.2f\n",
                      HPL_w[HPL_TIMING_LASWP-HPL_TIMING_BEG] );
/*
 * Upper triangular system solve
 * 上三角系统求解时间。
 */
      if( HPL_w[HPL_TIMING_PTRSV-HPL_TIMING_BEG] > HPL_rzero )
         HPL_fprintf( TEST->outfp,
                      "Max aggregated wall time up tr sv  . : %18.2f\n",
                      HPL_w[HPL_TIMING_PTRSV-HPL_TIMING_BEG] );

      if( TEST->thrsh <= HPL_rzero )
         HPL_fprintf( TEST->outfp, "%s%s\n",
                      "========================================",
                      "========================================" );
   }
#endif
/*
 * Quick return, if I am not interested in checking the computations
 * 如果当前测试不要求做正确性校验，则在输出性能结果后直接返回。
 */
   if( TEST->thrsh <= HPL_rzero )
   { (TEST->kpass)++; if( vptr ) free( vptr ); return; }
/*
 * Check info returned by solve
 * 检查求解器返回的状态码。
 *
 * mat.info 非零通常意味着数值分解过程中检测到奇异性或其他求解错误；
 * 这时继续做残差验证已经没有意义，因此直接把本轮记为 skip。
 */
   if( mat.info != 0 )
   {
      if( ( myrow == 0 ) && ( mycol == 0 ) )
         HPL_pwarn( TEST->outfp, __LINE__, "HPL_pdtest", "%s %d, %s", 
                    "Error code returned by solve is", mat.info, "skip" );
      (TEST->kskip)++;
      if( vptr ) free( vptr ); return;
   }
/*
 * Check computation, re-generate [ A | b ], compute norm 1 and inf of A and x,
 * and norm inf of b - A x. Display residual checks.
 * 重新生成 [ A | b ] 并执行结果校验：计算 A 与 x 的 1/inf 范数，以及
 * 残差 b - A x 的无穷范数，最后输出残差检查结果。
 */
   HPL_pdmatgen( GRID, N, N+1, NB, mat.A, mat.ld, HPL_ISEED );
   Anorm1 = HPL_pdlange( GRID, HPL_NORM_1, N, N, NB, mat.A, mat.ld );
   AnormI = HPL_pdlange( GRID, HPL_NORM_I, N, N, NB, mat.A, mat.ld );
/*
 * Because x is distributed in process rows, switch the norms.
 * 由于 x 是按进程行分布/复制的，所以这里对 1 范数与无穷范数的取法
 * 需要做一次“对调”解释。
 */
   XnormI = HPL_pdlange( GRID, HPL_NORM_1, 1, N, NB, mat.X, 1 );
   Xnorm1 = HPL_pdlange( GRID, HPL_NORM_I, 1, N, NB, mat.X, 1 );
/*
 * If I am in the col that owns b, (1) compute local BnormI, (2) all_reduce to
 * find the max (in the col). Then (3) broadcast along the rows so that every
 * process has BnormI. Note that since we use a uniform distribution in [-0.5,0.5]
 * for the entries of B, it is very likely that BnormI (<=,~) 0.5.
 * 如果当前进程位于拥有 b 的那一列，那么先计算本地 BnormI，再在该进程
 * 列上做 all-reduce 求最大值，最后沿进程行广播，让每个进程都拿到同一
 * 个 BnormI。由于 B 的元素来自 [-0.5, 0.5] 的均匀分布，BnormI 往往不
 * 会很大。
 *
 * This is a compact example of HPL's layered communication:
 * - HPL_all_reduce(..., HPL_max, GRID->col_comm) computes a column-wise
 *   collective max in the process column that owns b;
 * - HPL_broadcast(..., GRID->row_comm) then replicates the scalar across
 *   the process row so that every rank can evaluate the residual.
 * 这里很好地体现了 HPL 的分层通信：
 * - HPL_all_reduce(..., HPL_max, GRID->col_comm) 在拥有 b 的进程列上做
 *   一次按列的最大值归约；
 * - HPL_broadcast(..., GRID->row_comm) 再把这个标量沿进程行复制出去，
 *   这样每个 rank 都能进行残差评估。
 */
   Bptr = Mptr( mat.A, 0, nq, mat.ld );
   if( mycol == HPL_indxg2p( N, NB, NB, 0, npcol ) ){
      if( mat.mp > 0 )
      {
         BnormI = Bptr[HPL_idamax( mat.mp, Bptr, 1 )]; BnormI = Mabs( BnormI );
      }
      else
      {
         BnormI = HPL_rzero;
      }
      (void) HPL_all_reduce( (void *)(&BnormI), 1, HPL_DOUBLE, HPL_max,
                             GRID->col_comm );
   }
   (void) HPL_broadcast( (void *)(&BnormI), 1, HPL_DOUBLE,
                          HPL_indxg2p( N, NB, NB, 0, npcol ),
                          GRID->row_comm );
/*
 * If I own b, compute ( b - A x ) and ( - A x ) otherwise
 * 如果当前进程列拥有原始右端项 b，则就地计算 (b - A x)；
 * 否则仅计算本地贡献 (-A x)。
 *
 * 这样做的目的是让后续沿进程行执行 HPL_reduce(..., HPL_sum) 时，能把
 * 分散在不同进程列上的局部乘积项与真实 b 自动累加成完整残差向量。
 */
   if( mycol == HPL_indxg2p( N, NB, NB, 0, npcol ) )
   {
      HPL_dgemv( HplColumnMajor, HplNoTrans, mat.mp, nq, -HPL_rone,
                 mat.A, mat.ld, mat.X, 1, HPL_rone, Bptr, 1 );
   }
   else if( nq > 0 )
   {
      HPL_dgemv( HplColumnMajor, HplNoTrans, mat.mp, nq, -HPL_rone,
                 mat.A, mat.ld, mat.X, 1, HPL_rzero, Bptr, 1 );
   }
   else { for( ii = 0; ii < mat.mp; ii++ ) Bptr[ii] = HPL_rzero; }
/*
 * Reduce the distributed residual in process column 0.
 * 把分布式残差规约到进程列 0。
 *
 * Each process row contributes its local piece of b - A x, then a
 * row-wise HPL_reduce(..., HPL_sum, root=0) accumulates the distributed
 * vector in process column 0 before the final norm is computed.
 * 每条进程行先贡献自己持有的 b - A x 残差片段，然后通过
 * HPL_reduce(..., HPL_sum, root=0) 沿行规约到进程列 0，再计算最终范数。
 */
   if( mat.mp > 0 )
      (void) HPL_reduce( Bptr, mat.mp, HPL_DOUBLE, HPL_sum, 0,
                         GRID->row_comm );
/*
 * Compute || b - A x ||_oo
 * 计算残差向量 || b - A x ||_oo。
 */
   resid0 = HPL_pdlange( GRID, HPL_NORM_I, N, 1, NB, Bptr, mat.ld );
/*
 * Computes and displays norms, residuals ...
 * 计算并输出范数与残差判据。
 *
 * resid1 是 HPL 标准化后的最终测试比值，它综合考虑了机器精度 epsil、
 * 矩阵范数、解向量范数、右端项范数以及问题规模 N，用来判断这次求解在
 * 数值上是否可接受。
 */
   if( N <= 0 )
   {
      resid1 = HPL_rzero;
   }
   else
   {
      resid1 = resid0 / ( TEST->epsil * ( AnormI * XnormI + BnormI ) * (double)(N) );
   }

   if( resid1 < TEST->thrsh ) (TEST->kpass)++;
   else                       (TEST->kfail)++;

   if( ( myrow == 0 ) && ( mycol == 0 ) )
   {
      HPL_fprintf( TEST->outfp, "%s%s\n",
                   "----------------------------------------",
                   "----------------------------------------" );
      HPL_fprintf( TEST->outfp, "%s%16.8e%s%s\n",
         "||Ax-b||_oo/(eps*(||A||_oo*||x||_oo+||b||_oo)*N)= ", resid1,
         " ...... ", ( resid1 < TEST->thrsh ? "PASSED" : "FAILED" ) );

      if( resid1 >= TEST->thrsh ) 
      {
         HPL_fprintf( TEST->outfp, "%s%18.6f\n",
         "||Ax-b||_oo  . . . . . . . . . . . . . . . . . = ", resid0 );
         HPL_fprintf( TEST->outfp, "%s%18.6f\n",
         "||A||_oo . . . . . . . . . . . . . . . . . . . = ", AnormI );
         HPL_fprintf( TEST->outfp, "%s%18.6f\n",
         "||A||_1  . . . . . . . . . . . . . . . . . . . = ", Anorm1 );
         HPL_fprintf( TEST->outfp, "%s%18.6f\n",
         "||x||_oo . . . . . . . . . . . . . . . . . . . = ", XnormI );
         HPL_fprintf( TEST->outfp, "%s%18.6f\n",
         "||x||_1  . . . . . . . . . . . . . . . . . . . = ", Xnorm1 );
         HPL_fprintf( TEST->outfp, "%s%18.6f\n",
         "||b||_oo . . . . . . . . . . . . . . . . . . . = ", BnormI );
      }
   }
   if( vptr ) free( vptr );
/*
 * End of HPL_pdtest
 * HPL_pdtest 结束。
 */
}
