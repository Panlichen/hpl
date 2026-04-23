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
void HPL_pdgesv0
(
   HPL_T_grid *                     GRID,
   HPL_T_palg *                     ALGO,
   HPL_T_pmat *                     A
)
#else
void HPL_pdgesv0
( GRID, ALGO, A )
   HPL_T_grid *                     GRID;
   HPL_T_palg *                     ALGO;
   HPL_T_pmat *                     A;
#endif
{
/* 
 * Purpose
 * =======
 *
 * HPL_pdgesv0 factors a N+1-by-N matrix using LU factorization with row
 * partial pivoting.  The main algorithm  is the "right looking" variant
 * without look-ahead. The lower triangular factor is left unpivoted and
 * the pivots are not returned. The right hand side is the N+1 column of
 * the coefficient matrix.
 *
 * Arguments
 * =========
 *
 * GRID    (local input)                 HPL_T_grid *
 *         On entry,  GRID  points  to the data structure containing the
 *         process grid information.
 *
 * ALGO    (global input)                HPL_T_palg *
 *         On entry,  ALGO  points to  the data structure containing the
 *         algorithmic parameters.
 *
 * A       (local input/output)          HPL_T_pmat *
 *         On entry, A points to the data structure containing the local
 *         array information.
 *
 * ---------------------------------------------------------------------
 */ 
/*
 * .. Local Variables ..
 *
 * panel :
 *   single-entry panel pipeline used by the depth-0 schedule. panel[0]
 *   is repeatedly re-initialized to describe the current column panel.
 * panel：
 *   depth=0 调度下长度为 1 的 panel 流水线。panel[0] 会在每轮迭代中
 *   被重新初始化，用来描述当前列面板。
 *
 * HPL_pdupdate :
 *   concrete update kernel chosen in HPL_pddriver according to L1/U
 *   storage format; for the selected HPL.dat path it is HPL_pdupdateTT.
 * HPL_pdupdate：
 *   在 HPL_pddriver 中根据 L1/U 的存储格式选定的具体更新内核；对本次
 *   HPL.dat 选中的路径而言，它等于 HPL_pdupdateTT。
 *
 * N :
 *   global matrix order copied from A->n for loop control.
 * N：从 A->n 复制出的全局矩阵阶数，用于控制主循环。
 * j :
 *   current global column offset of the panel.
 * j：当前 panel 的全局列起点。
 * jb :
 *   actual width of the current panel, equal to min(remaining, nb).
 * jb：当前 panel 的真实宽度，等于 min(剩余列数, nb)。
 * n :
 *   remaining trailing-system size seen at offset j.
 * n：从当前偏移 j 开始还剩下的尾随子问题规模。
 * nb :
 *   algorithmic block size copied from the matrix descriptor.
 * nb：从矩阵描述符中读取出的算法块大小。
 * tag :
 *   rolling message id used by successive panel broadcasts.
 * tag：连续 panel 广播所使用的滚动消息编号。
 * test :
 *   polling state returned by HPL_bcast; loops until HPL_SUCCESS.
 * test：HPL_bcast 返回的轮询状态；直到变成 HPL_SUCCESS 才结束。
 */
   HPL_T_panel                * * panel = NULL;
   HPL_T_UPD_FUN              HPL_pdupdate;
   int                        N, j, jb, n, nb, tag=MSGID_BEGIN_FACT,
                              test=HPL_KEEP_TESTING;
#ifdef HPL_PROGRESS_REPORT
   double start_time, time, gflops;
#endif
/* ..
 * .. Executable Statements ..
 */
   if( ( N = A->n ) <= 0 ) return;

#ifdef HPL_PROGRESS_REPORT
   start_time = HPL_timer_walltime();
#endif
 
   HPL_pdupdate = ALGO->upfun; nb = A->nb;
/*
 * Allocate a panel list of length 1 - Allocate panel[0] resources.
 * 分配长度为 1 的 panel 列表，并为 panel[0] 分配资源。
 */
   panel = (HPL_T_panel **)malloc( sizeof( HPL_T_panel * ) );
   if( panel == NULL )
   { HPL_pabort( __LINE__, "HPL_pdgesv0", "Memory allocation failed" ); }

   HPL_pdpanel_new( GRID, ALGO, N, N+1, Mmin( N, nb ), A, 0, 0, tag,
                    &panel[0] );
/*
 * Loop over the columns of A.
 * 沿矩阵列方向推进主循环。
 *
 * Each iteration handles one panel of width jb:
 * 1. rebuild the panel descriptor over the current offset j;
 * 2. factor the panel in its owning process column;
 * 3. broadcast the packed panel along the owning process row;
 * 4. update the trailing matrix everywhere.
 * 每轮迭代处理一个宽度为 jb 的 panel：
 * 1. 以当前列偏移 j 重建 panel 描述符；
 * 2. 在拥有该 panel 的进程列中完成分解；
 * 3. 沿拥有该 panel 的进程行广播打包后的 panel；
 * 4. 在所有相关进程上更新尾随矩阵。
 *
 * With depth == 0 the four phases are intentionally serialized.
 * 当 depth == 0 时，这四个阶段会被有意串行化，不做 look-ahead 重叠。
 */
   for( j = 0; j < N; j += nb )
   {
      n = N - j; jb = Mmin( n, nb );
#ifdef HPL_PROGRESS_REPORT
      /* if this is process 0,0 and not the first panel */
      if ( GRID->myrow == 0 && GRID->mycol == 0 && j > 0 ) 
      {
          time = HPL_timer_walltime() - start_time;
          gflops = 2.0*(N*(double)N*N - n*(double)n*n)/3.0/(time > 0.0 ? time : 1e-6)/1e9;
          HPL_fprintf( stdout, "Column=%09d Fraction=%4.1f%% Gflops=%9.3e\n", j, j*100.0/N, gflops);
      }
#endif
/*
 * Release panel resources - re-initialize panel data structure.
 * 释放上一轮 panel 资源，并按当前偏移重新初始化 panel 描述符。
 */
      (void) HPL_pdpanel_free( panel[0] );
      HPL_pdpanel_init( GRID, ALGO, n, n+1, jb, A, j, j, tag, panel[0] );
/*
 * Factor and broadcast current panel - update.
 * 执行当前 panel 的分解、广播与尾随更新。
 *
 * HPL_binit/HPL_bcast/HPL_bwait are not MPI_Bcast wrappers.  They drive
 * a virtual row-broadcast topology selected at runtime (1-ring, 2-ring,
 * long, long-modified, ...), implemented on top of point-to-point MPI.
 * HPL_binit/HPL_bcast/HPL_bwait 并不是 MPI_Bcast 的简单封装，它们驱动
 * 的是运行期可选的虚拟行广播拓扑（1-ring、2-ring、long 等），底层实现
 * 仍然是点对点 MPI。
 */
      HPL_pdfact(               panel[0] );
      (void) HPL_binit(         panel[0] );
      do
      { (void) HPL_bcast(       panel[0], &test ); }
      while( test != HPL_SUCCESS );
      (void) HPL_bwait(         panel[0] );
/*
 * The update kernel consumes the pivot information generated during the
 * panel factorization, the row-wise broadcast payload, and the local
 * trailing blocks.  Depending on storage choices it dispatches to one of
 * HPL_pdupdate{NN,NT,TN,TT}.
 * 更新内核会消费 panel 分解产生的主元信息、按行广播得到的 panel 载荷，
 * 以及本地持有的尾随块。根据存储格式不同，它会落到
 * HPL_pdupdate{NN,NT,TN,TT} 之一。
 */
      HPL_pdupdate( NULL, NULL, panel[0], -1 );
/*
 * Update message id for next factorization
 */
      tag = MNxtMgid( tag, MSGID_BEGIN_FACT, MSGID_END_FACT );
   }
/*
 * Release panel resources and panel list
 */
   (void) HPL_pdpanel_disp( &panel[0] );

   if( panel ) free( panel );
/*
 * End of HPL_pdgesv0
 */
}
