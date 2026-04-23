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
int main
(
   int                        ARGC,
   char                       * * ARGV
)
#else
int main( ARGC, ARGV )
/*
 * .. Scalar Arguments ..
 */
   int                        ARGC;
/*
 * .. Array Arguments ..
 */
   char                       * * ARGV;
#endif
{
/* 
 * Purpose
 * =======
 *
 * main is the main driver program for testing the HPL routines.
 * This  program is  driven  by  a short data file named  "HPL.dat".
 *
 * ---------------------------------------------------------------------
 */ 
/*
 * .. Local Variables ..
 *
 * Parameter vectors populated by HPL_pdinfo from HPL.dat.
 * 由 HPL_pdinfo 从 HPL.dat 读取并填充的参数数组。
 *
 * nval   : candidate global matrix sizes N.
 * nval   ：候选的全局矩阵规模 N。
 * nbval  : candidate block sizes NB used by the 2D block-cyclic layout.
 * nbval  ：候选块大小 NB，用于二维块循环分布。
 * pval/qval : candidate process-grid dimensions P and Q.
 * pval/qval ：候选进程网格维度 P 和 Q。
 * nbmval : candidate NBMIN recursion stopping thresholds.
 * nbmval ：候选 NBMIN，控制递归何时切回非递归面板核。
 * ndvval : candidate NDIV recursion split factors.
 * ndvval ：候选 NDIV，控制递归时一个 panel 被切成多少份。
 * ndhval : candidate look-ahead depths.
 * ndhval ：候选 look-ahead 深度。
 */
   int                        nval  [HPL_MAX_PARAM],
                              nbval [HPL_MAX_PARAM],
                              pval  [HPL_MAX_PARAM],
                              qval  [HPL_MAX_PARAM],
                              nbmval[HPL_MAX_PARAM],
                              ndvval[HPL_MAX_PARAM],
                              ndhval[HPL_MAX_PARAM];

   HPL_T_FACT                 pfaval[HPL_MAX_PARAM],
                              rfaval[HPL_MAX_PARAM];

   HPL_T_TOP                  topval[HPL_MAX_PARAM];

   HPL_T_grid                 grid;   /* Active process-grid descriptor / 当前正在测试的进程网格描述符 */
   HPL_T_palg                 algo;   /* Algorithm bundle passed down to HPL_pdtest / 传递给 HPL_pdtest 的算法参数集合 */
   HPL_T_test                 test;   /* Output stream, thresholds, and pass/fail counters / 输出句柄、阈值以及测试计数器 */
/*
 * Scalar selectors and decoded configuration state.
 * 标量循环变量与解码后的配置状态。
 *
 * L1notran / Unotran :
 *   storage-format switches from HPL.dat; together they decide which
 *   update kernel HPL_pdupdate{NN,NT,TN,TT} is selected.
 * L1notran / Unotran：
 *   来自 HPL.dat 的存储格式开关；两者共同决定最终选择哪个
 *   HPL_pdupdate{NN,NT,TN,TT} 更新内核。
 *
 * align / equil :
 *   memory-alignment and equilibration controls copied into algo.
 * align / equil：
 *   内存对齐与均衡化控制，后续直接写入 algo。
 *
 * in/inb/inbm/indh/indv/ipfa/ipq/irfa/itop :
 *   loop indices over N, NB, NBMIN, DEPTH, NDIV, PFACT, P×Q, RFACT, and
 *   broadcast topology respectively.
 * in/inb/inbm/indh/indv/ipfa/ipq/irfa/itop：
 *   分别对应 N、NB、NBMIN、DEPTH、NDIV、PFACT、P×Q、RFACT、
 *   广播拓扑的枚举循环下标。
 *
 * myrow/mycol/nprow/npcol/rank/size :
 *   MPI rank identity in the global communicator and in the active grid.
 * myrow/mycol/nprow/npcol/rank/size：
 *   当前进程在全局通信器以及活动进程网格中的身份信息。
 *
 * ns/nbs/nbms/ndhs/ndvs/npfs/npqs/nrfs/ntps :
 *   actual list lengths returned by HPL_pdinfo.
 * ns/nbs/nbms/ndhs/ndvs/npfs/npqs/nrfs/ntps：
 *   HPL_pdinfo 返回的各配置列表长度。
 *
 * tswap / fswap :
 *   swap algorithm selector and threshold, consumed later by the update
 *   path when building row panel U.
 * tswap / fswap：
 *   行交换算法选择及阈值，后续在更新阶段构造行面板 U 时使用。
 */
   int                        L1notran, Unotran, align, equil, in, inb,
                              inbm, indh, indv, ipfa, ipq, irfa, itop,
                              mycol, myrow, ns, nbs, nbms, ndhs, ndvs,
                              npcol, npfs, npqs, nprow, nrfs, ntps, 
                              rank, size, tswap;
   HPL_T_ORDER                pmapping;
   HPL_T_FACT                 rpfa;
   HPL_T_SWAP                 fswap;
/* ..
 * .. Executable Statements ..
 */
   MPI_Init( &ARGC, &ARGV );
#ifdef HPL_CALL_VSIPL
   vsip_init((void*)0);
#endif
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );
/*
 * Read and check validity of test parameters from input file.
 * 读取并校验输入文件中的测试参数。
 *
 * The arrays above are filled with every candidate choice present in
 * HPL.dat, then the nested loops below enumerate the Cartesian product.
 * 上面的数组会被填充为 HPL.dat 中给出的全部候选项，随后下方的多重
 * 循环会枚举这些参数的笛卡尔积。
 */
   HPL_pdinfo( &test, &ns, nval, &nbs, nbval, &pmapping, &npqs, pval, qval,
               &npfs, pfaval, &nbms, nbmval, &ndvs, ndvval, &nrfs, rfaval,
               &ntps, topval, &ndhs, ndhval, &fswap, &tswap, &L1notran,
               &Unotran, &equil, &align );
/*
 * Loop over different process grids - Define process grid. Go to bottom
 * of process grid loop if this case does not use my process.
 * 遍历不同的进程网格并创建网格；如果当前进程不属于该网格，则直接
 * 跳到本轮进程网格循环尾部。
 *
 * This nested sweep is the experiment driver for HPL.  It enumerates
 * all algorithmic choices from HPL.dat, creates the requested process
 * grid, binds function pointers for factorization / recursive panel
 * factorization / update, and finally invokes HPL_pdtest().
 * 这一组嵌套循环就是 HPL 的实验驱动器：它枚举 HPL.dat 中的全部算法
 * 选项，创建所请求的进程网格，绑定分解 / 递归面板分解 / 更新函数指针，
 * 最后调用 HPL_pdtest() 执行一次完整求解与校验。
 */
   for( ipq = 0; ipq < npqs; ipq++ )
   {
      (void) HPL_grid_init( MPI_COMM_WORLD, pmapping, pval[ipq], qval[ipq],
                            &grid );  // src/grid/HPL_grid_init.c
      (void) HPL_grid_info( &grid, &nprow, &npcol, &myrow, &mycol );

      if( ( myrow < 0 ) || ( myrow >= nprow ) ||
          ( mycol < 0 ) || ( mycol >= npcol ) ) goto label_end_of_npqs;

      for( in = 0; in < ns; in++ )
      {                            /* Loop over various problem sizes */
       for( inb = 0; inb < nbs; inb++ )
       {                        /* Loop over various blocking factors */
        for( indh = 0; indh < ndhs; indh++ )
        {                       /* Loop over various lookahead depths */
         for( itop = 0; itop < ntps; itop++ )
         {                  /* Loop over various broadcast topologies */
          for( irfa = 0; irfa < nrfs; irfa++ )
          {             /* Loop over various recursive factorizations */
           for( ipfa = 0; ipfa < npfs; ipfa++ )
           {                /* Loop over various panel factorizations */
            for( inbm = 0; inbm < nbms; inbm++ )
            {        /* Loop over various recursive stopping criteria */
             for( indv = 0; indv < ndvs; indv++ )
             {          /* Loop over various # of panels in recursion */
/*
 * Set up the algorithm parameters.
 * 设置本轮测试的算法参数。
 *
 * The actual implementation is selected by function pointers:
 * - pffun / rffun choose the panel factorization kernels;
 * - upfun chooses the trailing update kernel and therefore the row-swap
 *   and U-panel communication path;
 * - btopo chooses the row-wise panel broadcast topology.
 * 真正被执行的实现是通过函数指针决定的：
 * - pffun / rffun 选择面板分解与递归面板分解内核；
 * - upfun 选择尾随更新内核，因此也间接决定行交换与 U 面板通信路径；
 * - btopo 选择按进程行执行的面板广播拓扑。
 */
              algo.btopo = topval[itop]; algo.depth = ndhval[indh];
              algo.nbmin = nbmval[inbm]; algo.nbdiv = ndvval[indv];

              algo.pfact = rpfa = pfaval[ipfa];

              if( L1notran != 0 )
              {
                 if( rpfa == HPL_LEFT_LOOKING ) algo.pffun = HPL_pdpanllN;
                 else if( rpfa == HPL_CROUT   ) algo.pffun = HPL_pdpancrN;
                 else                           algo.pffun = HPL_pdpanrlN;

                 algo.rfact = rpfa = rfaval[irfa];
                 if( rpfa == HPL_LEFT_LOOKING ) algo.rffun = HPL_pdrpanllN;
                 else if( rpfa == HPL_CROUT   ) algo.rffun = HPL_pdrpancrN;
                 else                           algo.rffun = HPL_pdrpanrlN;

                 if( Unotran != 0 ) algo.upfun = HPL_pdupdateNN;
                 else               algo.upfun = HPL_pdupdateNT;
              }
              else
              {
                 if( rpfa == HPL_LEFT_LOOKING ) algo.pffun = HPL_pdpanllT;
                 else if( rpfa == HPL_CROUT   ) algo.pffun = HPL_pdpancrT;
                 else                           algo.pffun = HPL_pdpanrlT;

                 algo.rfact = rpfa = rfaval[irfa];
                 if( rpfa == HPL_LEFT_LOOKING ) algo.rffun = HPL_pdrpanllT;
                 else if( rpfa == HPL_CROUT   ) algo.rffun = HPL_pdrpancrT;
                 else                           algo.rffun = HPL_pdrpanrlT;

                 if( Unotran != 0 ) algo.upfun = HPL_pdupdateTN;
                 else               algo.upfun = HPL_pdupdateTT;
              }

              algo.fswap = fswap; algo.fsthr = tswap;
              algo.equil = equil; algo.align = align;

              HPL_pdtest( &test, &grid, &algo, nval[in], nbval[inb] );

             }
            }
           }
          }
         }
        }
       }
      }
      (void) HPL_grid_exit( &grid );
label_end_of_npqs: ;
   }
/*
 * Print ending messages, close output file, exit.
 */
   if( rank == 0 )
   {
      test.ktest = test.kpass + test.kfail + test.kskip;
#ifndef HPL_DETAILED_TIMING
      HPL_fprintf( test.outfp, "%s%s\n",
                   "========================================",
                   "========================================" );
#else
      if( test.thrsh > HPL_rzero )
         HPL_fprintf( test.outfp, "%s%s\n",
                      "========================================",
                      "========================================" );
#endif

      HPL_fprintf( test.outfp, "\n%s %6d %s\n", "Finished", test.ktest,
                   "tests with the following results:" );
      if( test.thrsh > HPL_rzero )
      {
         HPL_fprintf( test.outfp, "         %6d %s\n", test.kpass,
                      "tests completed and passed residual checks," );
         HPL_fprintf( test.outfp, "         %6d %s\n", test.kfail,
                      "tests completed and failed residual checks," );
         HPL_fprintf( test.outfp, "         %6d %s\n", test.kskip,
                      "tests skipped because of illegal input values." );
      }
      else
      {
         HPL_fprintf( test.outfp, "         %6d %s\n", test.kpass,
                      "tests completed without checking," );
         HPL_fprintf( test.outfp, "         %6d %s\n", test.kskip,
                      "tests skipped because of illegal input values." );
      }

      HPL_fprintf( test.outfp, "%s%s\n",
                   "----------------------------------------",
                   "----------------------------------------" );
      HPL_fprintf( test.outfp, "\nEnd of Tests.\n" );
      HPL_fprintf( test.outfp, "%s%s\n",
                   "========================================",
                   "========================================" );

      if( ( test.outfp != stdout ) && ( test.outfp != stderr ) )
         (void) fclose( test.outfp );
   }
#ifdef HPL_CALL_VSIPL
   vsip_finalize((void*)0);
#endif
   MPI_Finalize();
   exit( 0 );

   return( 0 );
/*
 * End of main
 */
}
