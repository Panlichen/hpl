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
void HPL_pdtrsv
(
   HPL_T_grid *                     GRID,
   HPL_T_pmat *                     AMAT
)
#else
void HPL_pdtrsv
( GRID, AMAT )
   HPL_T_grid *                     GRID;
   HPL_T_pmat *                     AMAT;
#endif
{
/* 
 * Purpose
 * =======
 *
 * HPL_pdtrsv solves an upper triangular system of linear equations.
 *  
 * The rhs is the last column of the N by N+1 matrix A. The solve starts
 * in the process  column owning the  Nth  column of A, so the rhs b may
 * need to be moved one process column to the left at the beginning. The
 * routine therefore needs  a column  vector in every process column but
 * the one owning  b. The result is  replicated in all process rows, and
 * returned in XR, i.e. XR is of size nq = LOCq( N ) in all processes.
 *  
 * The algorithm uses decreasing one-ring broadcast in process rows  and
 * columns  implemented  in terms of  synchronous communication point to
 * point primitives.  The  lookahead of depth 1 is used to minimize  the
 * critical path. This entire operation is essentially ``latency'' bound
 * and an estimate of its running time is given by:
 *  
 *    (move rhs) lat + N / ( P bdwth ) +            
 *    (solve)    ((N / NB)-1) 2 (lat + NB / bdwth) +
 *               gam2 N^2 / ( P Q ),                
 *  
 * where  gam2   is an estimate of the   Level 2 BLAS rate of execution.
 * There are  N / NB  diagonal blocks. One must exchange  2  messages of
 * length NB to compute the next  NB  entries of the vector solution, as
 * well as performing a total of N^2 floating point operations.
 *
 * Arguments
 * =========
 *
 * GRID    (local input)                 HPL_T_grid *
 *         On entry,  GRID  points  to the data structure containing the
 *         process grid information.
 *
 * AMAT    (local input/output)          HPL_T_pmat *
 *         On entry,  AMAT  points  to the data structure containing the
 *         local array information.
 *
 * ---------------------------------------------------------------------
 */ 
/*
 * .. Local Variables ..
 *
 * Rcomm / Ccomm :
 *   row and column communicators used by the back-substitution pipeline.
 * Rcomm / Ccomm：回代流水线使用的行/列通信器。
 * A / Aptr / Aprev :
 *   base pointer to the local distributed matrix, the current diagonal
 *   block cursor, and the previous step's block cursor.
 * A / Aptr / Aprev：本地分布式矩阵基址、当前对角块游标以及上一步的块游标。
 * XR :
 *   local storage for the final solution vector replicated by process row.
 * XR：最终解向量在当前进程上的存储位置，会在进程行内复制。
 * XC :
 *   working right-hand-side / partial residual vector updated in place
 *   during backward substitution.
 * XC：回代过程中原地更新的工作右端项/部分残差向量。
 * Xd / Xdprev :
 *   current and previous solution blocks that are propagated through the
 *   pipeline.
 * Xd / Xdprev：当前与上一轮已经求出的解块，会在流水线中被传播。
 * W :
 *   temporary receive buffer for partial updates arriving from the
 *   previous owner column.
 * W：接收前一拥有列发来的部分更新时使用的临时缓冲区。
 * Alcol/Alrow :
 *   owner coordinates of the current diagonal block.
 * Alcol/Alrow：当前对角块的拥有列/拥有行。
 * Anp/Anq :
 *   local row/column counts of the active trailing matrix on this rank.
 * Anp/Anq：当前 rank 上活动尾随矩阵的本地行数/列数。
 * Bcol :
 *   process column that initially owns the extra rhs column b.
 * Bcol：最初拥有额外右端项列 b 的进程列。
 * Rmsgid/Cmsgid :
 *   rolling message tags for row-wise and column-wise point-to-point
 *   communications inside the solve.
 * Rmsgid/Cmsgid：回代过程中按行/按列点对点通信使用的滚动消息标签。
 * GridIsNotPx1 / GridIsNot1xQ :
 *   boolean shortcuts used to skip unnecessary sends on degenerate grids.
 * GridIsNotPx1 / GridIsNot1xQ：用于退化网格场景下跳过无效通信的布尔快捷标志。
 * kb/kbprev :
 *   width of the current / previous diagonal block.
 * kb/kbprev：当前/上一轮对角块的宽度。
 * n/n1/n1p/n1pprev :
 *   remaining unsolved system size and look-ahead window sizes for the
 *   latency-oriented pipeline.
 * n/n1/n1p/n1pprev：尚未回代的系统规模，以及低时延流水线使用的窗口长度。
 */
   MPI_Comm                   Ccomm, Rcomm;
   double                     * A=NULL, * Aprev=NULL, * Aptr, * XC=NULL,
                              * XR=NULL, * Xd=NULL, * Xdprev=NULL,
                              * W=NULL;
   int                        Alcol, Alrow, Anpprev, Anp, Anq, Bcol,
                              Cmsgid, GridIsNotPx1, GridIsNot1xQ, Rmsgid,
                              Wfr=0, colprev, kb, kbprev, lda, mycol,
                              myrow, n, n1, n1p, n1pprev=0, nb, npcol,
                              nprow, rowprev, tmp1, tmp2;
/* ..
 * .. Executable Statements ..
 */
#ifdef HPL_DETAILED_TIMING
   HPL_ptimer( HPL_TIMING_PTRSV );
#endif
   if( ( n = AMAT->n ) <= 0 ) return;
   nb = AMAT->nb; lda = AMAT->ld; A = AMAT->A; XR = AMAT->X;

   (void) HPL_grid_info( GRID, &nprow, &npcol, &myrow, &mycol );
   Rcomm = GRID->row_comm; Rmsgid = MSGID_BEGIN_PTRSV;
   Ccomm = GRID->col_comm; Cmsgid = MSGID_BEGIN_PTRSV + 1;
   GridIsNot1xQ = ( nprow > 1 ); GridIsNotPx1 = ( npcol > 1 );
/*
 * Move the rhs in the process column owning the last column of A.
 * 把右端项移动到拥有最后一个对角块的进程列。
 *
 * The right-hand side b is stored as the extra column of [A|b].  The
 * back solve starts from the process column owning the last diagonal
 * block, so HPL first performs a row-wise point-to-point relocation of b
 * to align the initial state with that owner column.
 * 右端项 b 存在 [A|b] 的最后一列。回代必须从拥有最后一个对角块的进程列
 * 开始，因此 HPL 会先沿进程行做一次点对点搬运，把 b 挪到正确的起始列。
 */
   Mnumroc( Anp, n, nb, nb, myrow, 0, nprow );
   Mnumroc( Anq, n, nb, nb, mycol, 0, npcol );

   tmp1  = ( n - 1 ) / nb;
   Alrow = tmp1 - ( tmp1 / nprow ) * nprow;
   Alcol = tmp1 - ( tmp1 / npcol ) * npcol;
   kb    = n    - tmp1 * nb;

   Aptr = (double *)(A); XC = Mptr( Aptr, 0, Anq, lda );
   Mindxg2p( n, nb, nb, Bcol, 0, npcol );

   if( ( Anp > 0 ) && ( Alcol != Bcol ) )
   {
      if( mycol == Bcol  )
      { (void) HPL_send( XC, Anp, Alcol, Rmsgid, Rcomm ); }
      else if( mycol == Alcol )
      { (void) HPL_recv( XC, Anp, Bcol,  Rmsgid, Rcomm ); }
   }
   Rmsgid = ( Rmsgid + 2 >
              MSGID_END_PTRSV ? MSGID_BEGIN_PTRSV : Rmsgid + 2 );
   if( mycol != Alcol )
   { for( tmp1=0; tmp1 < Anp; tmp1++ ) XC[tmp1] = HPL_rzero; }
/*
 * Set up lookahead
 */
   n1 = ( npcol - 1 ) * nb; n1 = Mmax( n1, nb );
   if( Anp > 0 )
   {
      W = (double*)malloc( (size_t)(Mmin( n1, Anp )) * sizeof( double ) );
      if( W == NULL )
      { HPL_pabort( __LINE__, "HPL_pdtrsv", "Memory allocation failed" ); }
      Wfr = 1;
   }

   Anpprev = Anp; Xdprev = XR; Aprev = Aptr = Mptr( Aptr, 0, Anq, lda );
   tmp1    = n - kb; tmp1 -= ( tmp2 = Mmin( tmp1, n1 ) );
   MnumrocI( n1pprev, tmp2, Mmax( 0, tmp1 ), nb, nb, myrow, 0, nprow );

   if( myrow == Alrow ) { Anpprev = ( Anp -= kb ); }
   if( mycol == Alcol )
   {
      Aprev = ( Aptr -= lda * kb ); Anq -= kb; Xdprev = ( Xd = XR + Anq );
      if( myrow == Alrow )
      {
         HPL_dtrsv( HplColumnMajor, HplUpper, HplNoTrans, HplNonUnit,
                    kb, Aptr+Anp, lda, XC+Anp, 1 );
         HPL_dcopy( kb, XC+Anp, 1, Xd, 1 );
      }
   }

   rowprev = Alrow; Alrow = MModSub1( Alrow, nprow );
   colprev = Alcol; Alcol = MModSub1( Alcol, npcol );
   kbprev  = kb; n -= kb;
   tmp1    = n - ( kb = nb ); tmp1 -= ( tmp2 = Mmin( tmp1, n1 ) );
   MnumrocI( n1p, tmp2, Mmax( 0, tmp1 ), nb, nb, myrow, 0, nprow );
/*
 * Start the operations.
 * 开始进入回代主循环。
 *
 * The main loop is a latency-oriented pipeline:
 * - previous solution block Xdprev is propagated in col_comm;
 * - partial updates XC are sent across row_comm to the next owner
 *   column;
 * - the owner of the current diagonal block performs a local DTRSV;
 * - bookkeeping advances to the next diagonal block on the upper-left.
 * 主循环是一个面向低时延的流水线：
 * - 上一轮解块 Xdprev 沿 col_comm 向上传播；
 * - 部分更新量 XC 沿 row_comm 发送给下一拥有列；
 * - 当前对角块的拥有者执行本地 DTRSV；
 * - 然后簿记状态推进到左上方下一个对角块。
 */
   while( n > 0 )
   {
      if( mycol == Alcol ) { Aptr -= lda * kb; Anq -= kb; Xd = XR + Anq; }
      if( myrow == Alrow ) { Anp -= kb; }
/*
 * Broadcast  (decreasing-ring)  of  previous solution block in previous
 * process column,  compute  partial update of current block and send it
 * to current process column.
 */
      if( mycol == colprev )
      {
/*
 * Send previous solution block in process row above
 */
         if( myrow == rowprev )
         {
            if( GridIsNot1xQ )
               (void) HPL_send( Xdprev, kbprev, MModSub1( myrow, nprow ),
                                Cmsgid, Ccomm );
         }
         else
         {
            (void) HPL_recv( Xdprev, kbprev, MModAdd1( myrow, nprow ),
                             Cmsgid, Ccomm );
         } 
/*
 * Compute partial update of previous solution block and send it to cur-
 * rent column
 */
         if( n1pprev > 0 )
         {
            tmp1 = Anpprev - n1pprev;
            HPL_dgemv( HplColumnMajor, HplNoTrans, n1pprev, kbprev,
                       -HPL_rone, Aprev+tmp1, lda, Xdprev, 1, HPL_rone,
                       XC+tmp1, 1 );
            if( GridIsNotPx1 )
               (void) HPL_send( XC+tmp1, n1pprev, Alcol, Rmsgid, Rcomm );
         }
/*
 * Finish  the (decreasing-ring) broadcast of the solution block in pre-
 * vious process column
 */
         if( ( myrow != rowprev ) &&
             ( myrow != MModAdd1( rowprev, nprow ) ) )
            (void) HPL_send( Xdprev, kbprev, MModSub1( myrow, nprow ),
                             Cmsgid, Ccomm );
      }
      else if( mycol == Alcol )
      {
/*
 * Current  column  receives  and accumulates partial update of previous
 * solution block
 */
         if( n1pprev > 0 )
         {
            (void) HPL_recv( W, n1pprev, colprev, Rmsgid, Rcomm );
            HPL_daxpy( n1pprev, HPL_rone, W, 1, XC+Anpprev-n1pprev, 1 );
         }
      }
/*
 * Solve current diagonal block 
 */
      if( ( mycol == Alcol ) && ( myrow == Alrow ) )
      {
         HPL_dtrsv( HplColumnMajor, HplUpper, HplNoTrans, HplNonUnit,
                    kb, Aptr+Anp, lda, XC+Anp, 1 );
         HPL_dcopy( kb, XC+Anp, 1, XR+Anq, 1 );
      }
/*
*  Finish previous update
*/
      if( ( mycol == colprev ) && ( ( tmp1 = Anpprev - n1pprev ) > 0 ) )
         HPL_dgemv( HplColumnMajor, HplNoTrans, tmp1, kbprev, -HPL_rone,
                    Aprev, lda, Xdprev, 1, HPL_rone, XC, 1 );
/*
*  Save info of current step and update info for the next step
*/
      if( mycol == Alcol ) { Xdprev   = Xd; Aprev = Aptr; }
      if( myrow == Alrow ) { Anpprev -= kb; }
      rowprev = Alrow; colprev = Alcol;
      n1pprev = n1p;   kbprev  = kb; n -= kb;
      Alrow = MModSub1( Alrow, nprow ); Alcol = MModSub1( Alcol, npcol );
      tmp1  = n - ( kb = nb ); tmp1 -= ( tmp2 = Mmin( tmp1, n1 ) );
      MnumrocI( n1p, tmp2, Mmax( 0, tmp1 ), nb, nb, myrow, 0, nprow );

      Rmsgid = ( Rmsgid+2 > MSGID_END_PTRSV ? 
                 MSGID_BEGIN_PTRSV   : Rmsgid+2 );
      Cmsgid = ( Cmsgid+2 > MSGID_END_PTRSV ?
                 MSGID_BEGIN_PTRSV+1 : Cmsgid+2 );
   }
/*
 * Replicate last solution block.
 * 复制最后一个解块。
 *
 * This is one of the few solve-phase operations with collective
 * semantics: once the last diagonal block has been solved, HPL_broadcast
 * replicates it through the column communicator so that XR is complete
 * in every process row.
 * 这是回代阶段少数带有“集合语义”的操作之一：当最后一个对角块求解完成
 * 后，HPL_broadcast 会沿列通信器复制该解块，从而让每一条进程行里的 XR
 * 都变得完整。
 */
   if( mycol == colprev )
      (void) HPL_broadcast( (void *)(XR), kbprev, HPL_DOUBLE, rowprev,
                            Ccomm );

   if( Wfr  ) free( W  );
#ifdef HPL_DETAILED_TIMING
   HPL_ptimer( HPL_TIMING_PTRSV );
#endif
/*
 * End of HPL_pdtrsv
 */
}
