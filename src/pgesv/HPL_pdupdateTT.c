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
void HPL_pdupdateTT
(
   HPL_T_panel *                    PBCST,
   int *                            IFLAG,
   HPL_T_panel *                    PANEL,
   const int                        NN
)
#else
void HPL_pdupdateTT
( PBCST, IFLAG, PANEL, NN )
   HPL_T_panel *                    PBCST;
   int *                            IFLAG;
   HPL_T_panel *                    PANEL;
   const int                        NN;
#endif
{
/* 
 * Purpose
 * =======
 *
 * HPL_pdupdateTT broadcast - forward the panel PBCST and simultaneously
 * applies the row interchanges and updates part of the trailing  (using
 * the panel PANEL) submatrix.
 *
 * Arguments
 * =========
 *
 * PBCST   (local input/output)          HPL_T_panel *
 *         On entry,  PBCST  points to the data structure containing the
 *         panel (to be broadcast) information.
 *
 * IFLAG   (local output)                int *
 *         On exit,  IFLAG  indicates  whether or not  the broadcast has
 *         been completed when PBCST is not NULL on entry. In that case,
 *         IFLAG is left unchanged.
 *
 * PANEL   (local input/output)          HPL_T_panel *
 *         On entry,  PANEL  points to the data structure containing the
 *         panel (to be updated) information.
 *
 * NN      (local input)                 const int
 *         On entry, NN specifies  the  local  number  of columns of the
 *         trailing  submatrix  to be updated  starting  at the  current
 *         position. NN must be at least zero.
 *
 * ---------------------------------------------------------------------
 */ 
/*
 * .. Local Variables ..
 *
 * Aptr :
 *   moving pointer to the current trailing block columns being updated.
 * Aptr：指向当前正在更新的尾随块列的滑动指针。
 * L1ptr / L2ptr :
 *   pointers to the replicated diagonal block and the rectangular
 *   subdiagonal block carried by PANEL.
 * L1ptr / L2ptr：指向 PANEL 中复制的对角块和矩形次对角块。
 * Uptr :
 *   pointer to the row panel U buffer used by the TT update path.
 * Uptr：TT 更新路径里保存行面板 U 的缓冲区指针。
 * dpiv / ipiv :
 *   replicated pivot array in double form and the integer workspace used
 *   after converting those pivots to local row offsets.
 * dpiv / ipiv：双精度形式复制出来的主元数组，以及把主元转成本地行偏移后
 *   使用的整型工作区。
 * curr :
 *   whether the current process row owns the current panel row block.
 * curr：当前进程行是否拥有当前 panel 的行块。
 * i/iroff :
 *   loop counter and row-offset baseline used when materializing pivots.
 * i/iroff：循环计数器，以及将主元转换为局部偏移时使用的基准行号。
 * jb/nb :
 *   panel width and algorithm block size.
 * jb/nb：panel 宽度与算法块大小。
 * lda/ldl2 :
 *   leading dimensions of A and L2.
 * lda/ldl2：A 与 L2 的主维。
 * mp/n/nq0/nn :
 *   local row count below the panel, total columns to update, number of
 *   columns already updated, and the chunk size processed this round.
 * mp/n/nq0/nn：panel 下方本地行数、待更新总列数、已更新列数、当前轮分块
 *   更新的列数。
 * test :
 *   polling state returned by HPL_bcast while overlap is in progress.
 * test：look-ahead/重叠过程中由 HPL_bcast 返回的轮询状态。
 * tswap / fswap :
 *   cached swap policy and threshold so the branch is resolved once.
 * tswap / fswap：缓存下来的交换策略与阈值，避免每次都重复解析配置。
 */
   double                    * Aptr, * L1ptr, * L2ptr, * Uptr, * dpiv;
   int                       * ipiv;
#ifdef HPL_CALL_VSIPL
   vsip_mview_d              * Av0, * Av1, * Lv0, * Lv1, * Uv0, * Uv1;
#endif
   int                       curr, i, iroff, jb, lda, ldl2, mp, n, nb,
                             nq0, nn, test;
   static int                tswap = 0;
   static HPL_T_SWAP         fswap = HPL_NO_SWP;
#define LDU                  n
/* ..
 * .. Executable Statements ..
 */
#ifdef HPL_DETAILED_TIMING
   HPL_ptimer( HPL_TIMING_UPDATE );
#endif
   nb = PANEL->nb; jb = PANEL->jb; n = PANEL->nq; lda = PANEL->lda;
   if( NN >= 0 ) n = Mmin( NN, n );
/*
 * There is nothing to update, enforce the panel broadcast.
 * 如果没有尾随列需要更新，就只负责把尚未完成的 panel 广播推进到完成。
 */
   if( ( n <= 0 ) || ( jb <= 0 ) )
   {
      if( PBCST != NULL )
      {
         do { (void) HPL_bcast( PBCST, IFLAG ); }
         while( *IFLAG != HPL_SUCCESS );
      }
#ifdef HPL_DETAILED_TIMING
      HPL_ptimer( HPL_TIMING_UPDATE );
#endif
      return;
   }
/*
 * Enable/disable the column panel probing mechanism.
 * 触发对下一块 panel 广播进度的探测/推进机制。
 */
   (void) HPL_bcast( PBCST, &test );
/*
 * 1 x Q case
 */
   if( PANEL->grid->nprow == 1 )
   {
      Aptr = PANEL->A;       L2ptr = PANEL->L2;   L1ptr = PANEL->L1;
      ldl2 = PANEL->ldl2;    dpiv  = PANEL->DPIV; ipiv  = PANEL->IWORK;
      mp   = PANEL->mp - jb; iroff = PANEL->ii;   nq0   = 0; 
#ifdef HPL_CALL_VSIPL
/*
 * Admit the blocks
 */
      (void) vsip_blockadmit_d(  PANEL->Ablock,  VSIP_TRUE );
      (void) vsip_blockadmit_d(  PANEL->L2block, VSIP_TRUE );
/*
 * Create the matrix views
 */
      Av0 = vsip_mbind_d( PANEL->Ablock,  0, 1, lda,  lda,  PANEL->pmat->nq );
      Lv0 = vsip_mbind_d( PANEL->L2block, 0, 1, ldl2, ldl2,              jb );
/*
 * Create the matrix subviews
 */
      Lv1 = vsip_msubview_d( Lv0, 0, 0, mp, jb );
#endif
      for( i = 0; i < jb; i++ ) { ipiv[i] = (int)(dpiv[i]) - iroff; }
/*
 * So far we have not updated anything -  test availability of the panel
 * to be forwarded - If detected forward it and finish the update in one
 * step.
 */
      while ( test == HPL_KEEP_TESTING )
      {
         nn = n - nq0; nn = Mmin( nb, nn );
/*
 * Update nb columns at a time
 */
#ifdef HPL_DETAILED_TIMING
         HPL_ptimer( HPL_TIMING_LASWP );
         HPL_dlaswp00N( jb, nn, Aptr, lda, ipiv );
         HPL_ptimer( HPL_TIMING_LASWP );
#else
         HPL_dlaswp00N( jb, nn, Aptr, lda, ipiv );
#endif
         HPL_dtrsm( HplColumnMajor, HplLeft, HplUpper, HplTrans,
                    HplUnit, jb, nn, HPL_rone, L1ptr, jb, Aptr, lda );
#ifdef HPL_CALL_VSIPL
/*
 * Create the matrix subviews
 */
         Uv1 = vsip_msubview_d( Av0, PANEL->ii,    PANEL->jj+nq0, jb, nn );
         Av1 = vsip_msubview_d( Av0, PANEL->ii+jb, PANEL->jj+nq0, mp, nn );

         vsip_gemp_d( -HPL_rone, Lv1, VSIP_MAT_NTRANS, Uv1, VSIP_MAT_NTRANS,
                      HPL_rone, Av1 );
/*
 * Destroy the matrix subviews
 */
         (void) vsip_mdestroy_d( Av1 );
         (void) vsip_mdestroy_d( Uv1 );
#else
         HPL_dgemm( HplColumnMajor, HplNoTrans, HplNoTrans, mp, nn,
                    jb, -HPL_rone, L2ptr, ldl2, Aptr, lda, HPL_rone,
                    Mptr( Aptr, jb, 0, lda ), lda );
#endif
         Aptr = Mptr( Aptr, 0, nn, lda ); nq0 += nn; 

         (void) HPL_bcast( PBCST, &test ); 
      }
/*
 * The panel has been forwarded at that point, finish the update
 */
      if( ( nn = n - nq0 ) > 0 )
      {
#ifdef HPL_DETAILED_TIMING
         HPL_ptimer( HPL_TIMING_LASWP );
         HPL_dlaswp00N( jb, nn, Aptr, lda, ipiv );
         HPL_ptimer( HPL_TIMING_LASWP );
#else
         HPL_dlaswp00N( jb, nn, Aptr, lda, ipiv );
#endif
         HPL_dtrsm( HplColumnMajor, HplLeft, HplUpper, HplTrans,
                    HplUnit, jb, nn, HPL_rone, L1ptr, jb, Aptr, lda );
#ifdef HPL_CALL_VSIPL
/*
 * Create the matrix subviews
 */
         Uv1 = vsip_msubview_d( Av0, PANEL->ii,    PANEL->jj+nq0, jb, nn );
         Av1 = vsip_msubview_d( Av0, PANEL->ii+jb, PANEL->jj+nq0, mp, nn );

         vsip_gemp_d( -HPL_rone, Lv1, VSIP_MAT_NTRANS, Uv1, VSIP_MAT_NTRANS,
                      HPL_rone, Av1 );
/*
 * Destroy the matrix subviews
 */
         (void) vsip_mdestroy_d( Av1 );
         (void) vsip_mdestroy_d( Uv1 );
#else
         HPL_dgemm( HplColumnMajor, HplNoTrans, HplNoTrans, mp, nn,
                    jb, -HPL_rone, L2ptr, ldl2, Aptr, lda, HPL_rone,
                    Mptr( Aptr, jb, 0, lda ), lda );
#endif
      }
#ifdef HPL_CALL_VSIPL
/*
 * Destroy the matrix subviews
 */
      (void) vsip_mdestroy_d( Lv1 );
/*
 * Release the blocks
 */
      (void) vsip_blockrelease_d( vsip_mgetblock_d( Lv0 ), VSIP_TRUE );
      (void) vsip_blockrelease_d( vsip_mgetblock_d( Av0 ), VSIP_TRUE );
/*
 * Destroy the matrix views
 */
      (void) vsip_mdestroy_d( Lv0 );
      (void) vsip_mdestroy_d( Av0 );
#endif
   }
   else                        /* nprow > 1 ... */
   {
/*
 * Selection of the swapping algorithm - swap:broadcast U.
 * 选择行交换算法，并据此执行“交换 + 广播 U”路径。
 */
      if( fswap == HPL_NO_SWP )
      { fswap = PANEL->algo->fswap; tswap = PANEL->algo->fsthr; }

      if( (   fswap == HPL_SWAP01 ) ||
          ( ( fswap == HPL_SW_MIX ) && ( n > tswap ) ) )
      { HPL_pdlaswp01T( PBCST, &test, PANEL, n ); }
      else
      { HPL_pdlaswp00T( PBCST, &test, PANEL, n ); }
/*
 * Compute redundantly row block of U and update trailing submatrix.
 * 在各进程行上冗余计算 U 的行块，并用它更新尾随子矩阵。
 */
      nq0 = 0; curr = ( PANEL->grid->myrow == PANEL->prow ? 1 : 0 );
      Aptr = PANEL->A; L2ptr = PANEL->L2;  L1ptr = PANEL->L1;
      Uptr = PANEL->U; ldl2 = PANEL->ldl2;
      mp   = PANEL->mp - ( curr != 0 ? jb : 0 );
#ifdef HPL_CALL_VSIPL
/*
 * Admit the blocks
 */
      (void) vsip_blockadmit_d(  PANEL->Ablock,  VSIP_TRUE );
      (void) vsip_blockadmit_d(  PANEL->L2block, VSIP_TRUE );
      (void) vsip_blockadmit_d(  PANEL->Ublock,  VSIP_TRUE );
/*
 * Create the matrix views
 */
      Av0 = vsip_mbind_d( PANEL->Ablock,  0, 1, lda,  lda,  PANEL->pmat->nq );
      Lv0 = vsip_mbind_d( PANEL->L2block, 0, 1, ldl2, ldl2,              jb );
      Uv0 = vsip_mbind_d( PANEL->Ublock,  0, 1, LDU,  LDU,               jb );
/*
 * Create the matrix subviews
 */
      Lv1 = vsip_msubview_d( Lv0, 0, 0, mp, jb );
#endif
/*
 * Broadcast has not occured yet, spliting the computational part.
 * panel 广播尚未完成时，把计算拆成按 nb 列的小块，边计算边轮询广播。
 */
      while ( test == HPL_KEEP_TESTING )
      {
         nn = n - nq0; nn = Mmin( nb, nn );

         HPL_dtrsm( HplColumnMajor, HplRight, HplUpper, HplNoTrans,
                    HplUnit, nn, jb, HPL_rone, L1ptr, jb, Uptr, LDU );

         if( curr != 0 )
         {
#ifdef HPL_CALL_VSIPL
/*
 * Create the matrix subviews
 */
            Uv1 = vsip_msubview_d( Uv0, nq0,          0,             nn, jb );
            Av1 = vsip_msubview_d( Av0, PANEL->ii+jb, PANEL->jj+nq0, mp, nn );

            vsip_gemp_d( -HPL_rone, Lv1, VSIP_MAT_NTRANS, Uv1, VSIP_MAT_TRANS,
                         HPL_rone, Av1 );
/*
 * Destroy the matrix subviews
 */
            (void) vsip_mdestroy_d( Av1 );
            (void) vsip_mdestroy_d( Uv1 );
#else
            HPL_dgemm( HplColumnMajor, HplNoTrans, HplTrans, mp, nn,
                       jb, -HPL_rone, L2ptr, ldl2, Uptr, LDU, HPL_rone,
                       Mptr( Aptr, jb, 0, lda ), lda );
#endif
            HPL_dlatcpy( jb, nn, Uptr, LDU, Aptr, lda );
         }
         else
         {
#ifdef HPL_CALL_VSIPL
/*
 * Create the matrix subviews
 */
            Uv1 = vsip_msubview_d( Uv0, nq0,          0,             nn, jb );
            Av1 = vsip_msubview_d( Av0, PANEL->ii,    PANEL->jj+nq0, mp, nn );

            vsip_gemp_d( -HPL_rone, Lv1, VSIP_MAT_NTRANS, Uv1, VSIP_MAT_TRANS,
                         HPL_rone, Av1 );
/*
 * Destroy the matrix subviews
 */
            (void) vsip_mdestroy_d( Av1 );
            (void) vsip_mdestroy_d( Uv1 );
#else
            HPL_dgemm( HplColumnMajor, HplNoTrans, HplTrans, mp, nn,
                       jb, -HPL_rone, L2ptr, ldl2, Uptr, LDU, HPL_rone,
                       Aptr, lda );
#endif
         }
         Uptr = Mptr( Uptr, nn, 0, LDU );
         Aptr = Mptr( Aptr, 0, nn, lda ); nq0 += nn;

         (void) HPL_bcast( PBCST, &test ); 
      }
/*
 * The panel has been forwarded at that point, finish the update.
 * 此时 panel 已经完成转发，可以把剩余尾随更新一次性做完。
 */
      if( ( nn = n - nq0 ) > 0 )
      {
         HPL_dtrsm( HplColumnMajor, HplRight, HplUpper, HplNoTrans,
                    HplUnit, nn, jb, HPL_rone, L1ptr, jb, Uptr, LDU );

         if( curr != 0 )
         {
#ifdef HPL_CALL_VSIPL
/*
 * Create the matrix subviews
 */
            Uv1 = vsip_msubview_d( Uv0, nq0,          0,             nn, jb );
            Av1 = vsip_msubview_d( Av0, PANEL->ii+jb, PANEL->jj+nq0, mp, nn );

            vsip_gemp_d( -HPL_rone, Lv1, VSIP_MAT_NTRANS, Uv1, VSIP_MAT_TRANS,
                         HPL_rone, Av1 );
/*
 * Destroy the matrix subviews
 */
            (void) vsip_mdestroy_d( Av1 );
            (void) vsip_mdestroy_d( Uv1 );
#else
            HPL_dgemm( HplColumnMajor, HplNoTrans, HplTrans, mp, nn,
                       jb, -HPL_rone, L2ptr, ldl2, Uptr, LDU, HPL_rone,
                       Mptr( Aptr, jb, 0, lda ), lda );
#endif
            HPL_dlatcpy( jb, nn, Uptr, LDU, Aptr, lda );
         }
         else
         {
#ifdef HPL_CALL_VSIPL
/*
 * Create the matrix subviews
 */
            Uv1 = vsip_msubview_d( Uv0, nq0,          0,             nn, jb );
            Av1 = vsip_msubview_d( Av0, PANEL->ii,    PANEL->jj+nq0, mp, nn );

            vsip_gemp_d( -HPL_rone, Lv1, VSIP_MAT_NTRANS, Uv1, VSIP_MAT_TRANS,
                         HPL_rone, Av1 );
/*
 * Destroy the matrix subviews
 */
            (void) vsip_mdestroy_d( Av1 );
            (void) vsip_mdestroy_d( Uv1 );
#else
            HPL_dgemm( HplColumnMajor, HplNoTrans, HplTrans, mp, nn,
                       jb, -HPL_rone, L2ptr, ldl2, Uptr, LDU, HPL_rone,
                       Aptr, lda );
#endif
         }
      }
#ifdef HPL_CALL_VSIPL
/*
 * Destroy the matrix subviews
 */
      (void) vsip_mdestroy_d( Lv1 );
/*
 * Release the blocks
 */
      (void) vsip_blockrelease_d( vsip_mgetblock_d( Uv0 ), VSIP_TRUE );
      (void) vsip_blockrelease_d( vsip_mgetblock_d( Lv0 ), VSIP_TRUE );
      (void) vsip_blockrelease_d( vsip_mgetblock_d( Av0 ), VSIP_TRUE );
/*
 * Destroy the matrix views
 */
      (void) vsip_mdestroy_d( Uv0 );
      (void) vsip_mdestroy_d( Lv0 );
      (void) vsip_mdestroy_d( Av0 );
#endif
   }

   PANEL->A = Mptr( PANEL->A, 0, n, lda ); PANEL->nq -= n; PANEL->jj += n;
/*
 * return the outcome of the probe  (should always be  HPL_SUCCESS,  the
 * panel broadcast is enforced in that routine).
 * 返回轮询/探测的最终结果；理论上这里应该已经被推进到 HPL_SUCCESS，
 * 因为本例程内部会强制把 panel 广播完成。
 */
   if( PBCST != NULL ) *IFLAG = test;
#ifdef HPL_DETAILED_TIMING
   HPL_ptimer( HPL_TIMING_UPDATE );
#endif
/*
 * End of HPL_pdupdateTT
 */
}
