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
void HPL_pdfact
(
   HPL_T_panel *                    PANEL
)
#else
void HPL_pdfact
( PANEL )
   HPL_T_panel *                    PANEL;
#endif
{
/* 
 * Purpose
 * =======
 *
 * HPL_pdfact recursively factorizes a  1-dimensional  panel of columns.
 * The  RPFACT  function pointer specifies the recursive algorithm to be
 * used, either Crout, Left- or Right looking.  NBMIN allows to vary the
 * recursive stopping criterium in terms of the number of columns in the
 * panel, and  NDIV allows to specify the number of subpanels each panel
 * should be divided into. Usuallly a value of 2 will be chosen. Finally
 * PFACT is a function pointer specifying the non-recursive algorithm to
 * to be used on at most NBMIN columns. One can also choose here between
 * Crout, Left- or Right looking.  Empirical tests seem to indicate that
 * values of 4 or 8 for NBMIN give the best results.
 *  
 * Bi-directional  exchange  is  used  to  perform  the  swap::broadcast
 * operations  at once  for one column in the panel.  This  results in a
 * lower number of slightly larger  messages than usual.  On P processes
 * and assuming bi-directional links,  the running time of this function
 * can be approximated by (when N is equal to N0):                      
 *  
 *    N0 * log_2( P ) * ( lat + ( 2*N0 + 4 ) / bdwth ) +
 *    N0^2 * ( M - N0/3 ) * gam2-3
 *  
 * where M is the local number of rows of  the panel, lat and bdwth  are
 * the latency and bandwidth of the network for  double  precision  real
 * words, and  gam2-3  is  an estimate of the  Level 2 and Level 3  BLAS
 * rate of execution. The  recursive  algorithm  allows indeed to almost
 * achieve  Level 3 BLAS  performance  in the panel factorization.  On a
 * large  number of modern machines,  this  operation is however latency
 * bound,  meaning  that its cost can  be estimated  by only the latency
 * portion N0 * log_2(P) * lat.  Mono-directional links will double this
 * communication cost.
 *
 * Arguments
 * =========
 *
 * PANEL   (local input/output)          HPL_T_panel *
 *         On entry,  PANEL  points to the data structure containing the
 *         panel information.
 *
 * ---------------------------------------------------------------------
 */ 
/*
 * .. Local Variables ..
 *
 * vptr :
 *   scratch allocation used to hold the temporary work array passed into
 *   the recursive panel factorization kernel.
 * vptr：
 *   临时工作内存，作为递归面板分解内核的 WORK 参数传入。
 *
 * align :
 *   alignment requirement copied from algo for the temporary workspace.
 * align：从 algo 中复制出的对齐要求，用于临时工作区。
 * jb :
 *   width of the current panel; also the number of columns removed from
 *   the trailing matrix after this factorization finishes.
 * jb：当前 panel 的宽度；分解完成后也正是从尾随矩阵中“剥离”的列数。
 */
   void                       * vptr = NULL;
   int                        align, jb;
/* ..
 * .. Executable Statements ..
 */
   jb = PANEL->jb; PANEL->n -= jb; PANEL->ja += jb;

/*
 * Only the process column owning the current panel performs the actual
 * panel factorization.  Other process columns learn the results through
 * the subsequent row-wise panel broadcast.
 * 只有拥有当前 panel 的进程列会真正执行面板分解；其他进程列不会重复
 * 做这一步，而是通过后续按行的 panel 广播获得结果。
 */
   if( ( PANEL->grid->mycol != PANEL->pcol ) || ( jb <= 0 ) ) return;
#ifdef HPL_DETAILED_TIMING
   HPL_ptimer( HPL_TIMING_RPFACT );
#endif
   align = PANEL->algo->align;
   vptr  = (void *)malloc( ( (size_t)(align) + 
              (size_t)(((4+((unsigned int)(jb) << 1)) << 1) )) *
              sizeof(double) );
   if( vptr == NULL )
   { HPL_pabort( __LINE__, "HPL_pdfact", "Memory allocation failed" ); }
/*
 * Factor the panel - Update the panel pointers.
 * 分解 panel，并把 panel 描述符推进到下一个尾随区域。
 *
 * rffun is the recursive panel kernel selected in the driver.  Deep in
 * that call tree HPL repeatedly performs local pivot search, exchanges
 * pivot candidates / rows within one process column, and updates the
 * local panel block with BLAS.
 * rffun 是驱动层根据配置选出的递归面板分解内核。在它的调用树内部，
 * HPL 会反复执行局部主元搜索、同一进程列内的候选主元/行交换，以及
 * 基于 BLAS 的局部 panel 更新。
 */
   PANEL->algo->rffun( PANEL, PANEL->mp, jb, 0, (double *)HPL_PTR( vptr,
                       ((size_t)(align) * sizeof(double) ) ) );
   if( vptr ) free( vptr );

   PANEL->A   = Mptr( PANEL->A, 0, jb, PANEL->lda );
   PANEL->nq -= jb; PANEL->jj += jb;
#ifdef HPL_DETAILED_TIMING
   HPL_ptimer( HPL_TIMING_RPFACT );
#endif
/*
 * End of HPL_pdfact
 */
}
