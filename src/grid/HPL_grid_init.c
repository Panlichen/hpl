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
int HPL_grid_init
(
   MPI_Comm                         COMM,
   const HPL_T_ORDER                ORDER,
   const int                        NPROW,
   const int                        NPCOL,
   HPL_T_grid *                     GRID
)
#else
int HPL_grid_init
( COMM, ORDER, NPROW, NPCOL, GRID )
   MPI_Comm                         COMM;
   const HPL_T_ORDER                ORDER;
   const int                        NPROW;
   const int                        NPCOL;
   HPL_T_grid *                     GRID;
#endif
{
/* 
 * Purpose
 * =======
 *
 * HPL_grid_init creates a NPROW x NPCOL  process  grid using column- or
 * row-major ordering from an initial collection of processes identified
 * by an  MPI  communicator.  Successful  completion is indicated by the
 * returned error code MPI_SUCCESS.  Other error codes depend on the MPI
 * implementation. The coordinates of processes that are not part of the
 * grid are set to values outside of [0..NPROW) x [0..NPCOL).
 *
 * HPL_grid_init 从一个由 MPI 通信器标识的初始进程集合中，按照行主序
 * 或列主序创建一个 NPROW x NPCOL 的进程网格。成功完成时返回
 * MPI_SUCCESS，其他错误码取决于具体的 MPI 实现。对于不属于该网格的
 * 进程，其坐标会被设置到 [0..NPROW) x [0..NPCOL) 之外。
 *
 * Arguments
 * =========
 * 参数说明
 * ========
 *
 * COMM    (global/local input)          MPI_Comm
 *         On entry,  COMM  is  the  MPI  communicator  identifying  the
 *         initial  collection  of  processes out of which  the  grid is
 *         formed.
 *         输入时，COMM 是标识初始进程集合的 MPI 通信器，后续的网格
 *         就是从这个进程集合中切分出来的。
 *
 * ORDER   (global input)                const HPL_T_ORDER
 *         On entry, ORDER specifies how the processes should be ordered
 *         in the grid as follows:
 *            ORDER = HPL_ROW_MAJOR    row-major    ordering;
 *            ORDER = HPL_COLUMN_MAJOR column-major ordering;
 *         输入时，ORDER 指定进程在网格中的编号方式：
 *            ORDER = HPL_ROW_MAJOR    按行主序编号；
 *            ORDER = HPL_COLUMN_MAJOR 按列主序编号。
 *
 * NPROW   (global input)                const int
 *         On entry,  NPROW  specifies the number of process rows in the
 *         grid to be created. NPROW must be at least one.
 *         输入时，NPROW 给出要创建的进程网格行数，必须至少为 1。
 *
 * NPCOL   (global input)                const int
 *         On entry,  NPCOL  specifies  the number of process columns in
 *         the grid to be created. NPCOL must be at least one.
 *         输入时，NPCOL 给出要创建的进程网格列数，必须至少为 1。
 *
 * GRID    (local input/output)          HPL_T_grid *
 *         On entry,  GRID  points  to the data structure containing the
 *         process grid information to be initialized.
 *         输入时，GRID 指向需要被初始化的进程网格描述符。
 *
 * ---------------------------------------------------------------------
 */ 
/*
 * .. Local Variables ..
 *
 * hdim :
 *   hypercube dimension associated with the largest power-of-two subset
 *   of rows or columns.  It is stored into GRID->row_hdim / col_hdim and
 *   later used by HPL's custom collective schedules.
 * hdim：
 *   与“最大 2 的幂规模子集”对应的超立方维度。它会被写入
 *   GRID->row_hdim / col_hdim，后续供 HPL 的自定义广播/归约调度使用。
 *
 * hplerr :
 *   sticky return code for communicator-creation steps.  Even if one of
 *   the splits fails, the routine tries to finish the remaining setup and
 *   returns the first non-success code seen.
 * hplerr：
 *   通信器创建阶段的累计错误码。即使某一步 split 失败，本例程仍尽量完
 *   成剩余初始化，并把遇到的非成功状态返回给调用者。
 *
 * ierr :
 *   immediate return code from each MPI call.
 * ierr：
 *   每次 MPI 调用返回的即时状态码。
 *
 * ip2 :
 *   running largest-power-of-two value used when deriving row_ip2,
 *   row_ip2m1, col_ip2, and col_ip2m1.
 * ip2：
 *   计算最大 2 的幂辅助量时使用的滚动变量，最终用于得到 row_ip2、
 *   row_ip2m1、col_ip2 和 col_ip2m1。
 *
 * k :
 *   temporary shrinking counter used in the repeated right-shift loops
 *   that derive power-of-two helper values.
 * k：
 *   在不断右移的循环中使用的临时计数器，用来推导各种 2 的幂辅助参数。
 *
 * mask :
 *   hypercube mask corresponding to the largest power of two not exceeding
 *   nprow-1 or npcol-1.  It is later consumed by HPL's tree / exchange
 *   communication helpers.
 * mask：
 *   对应于“不超过 nprow-1 或 npcol-1 的最大 2 的幂”的超立方掩码，
 *   后续会被 HPL 的树形/交换通信辅助逻辑使用。
 *
 * myrow / mycol :
 *   logical coordinates assigned to the current MPI rank inside the target
 *   process grid.  These values are stored into GRID and are the basis of
 *   every later row-wise / column-wise communication split.
 * myrow / mycol：
 *   当前 MPI rank 在目标进程网格中的逻辑坐标。这两个值会写入 GRID，
 *   是后续所有按行/按列通信器切分的基础。
 *
 * nprocs :
 *   total number of MPI ranks actually used by the grid, equal to
 *   NPROW * NPCOL.
 * nprocs：
 *   当前网格实际需要使用的 MPI 进程总数，等于 NPROW * NPCOL。
 *
 * rank / size :
 *   rank id and communicator size inside the incoming communicator COMM.
 *   They are used to decide whether this process participates in the grid
 *   and how its coordinates are mapped.
 * rank / size：
 *   当前进程在输入通信器 COMM 中的 rank 以及通信器总规模，用来判断该
 *   进程是否参与网格，以及它在网格中的坐标如何映射。
 */
   int                        hdim, hplerr=MPI_SUCCESS, ierr, ip2, k,
                              mask, mycol, myrow, nprocs, rank, size;
/* ..
 * .. Executable Statements ..
 */
   MPI_Comm_rank( COMM, &rank ); MPI_Comm_size( COMM, &size );
/*
 * Abort if illegal process grid.
 * 如果请求的进程网格不合法，则立即终止。
 */
   nprocs = NPROW * NPCOL;
   if( ( nprocs > size ) || ( NPROW < 1 ) || ( NPCOL < 1 ) )
   { HPL_pabort( __LINE__, "HPL_grid_init", "Illegal Grid" ); }
/*
 * Row- or column-major ordering of the processes.
 * 按行主序或列主序把全局 rank 映射到目标网格坐标。
 */
   if( ORDER == HPL_ROW_MAJOR )
   {
      GRID->order = HPL_ROW_MAJOR;
      myrow = rank / NPCOL; mycol = rank - myrow * NPCOL;
   }
   else
   {
      GRID->order = HPL_COLUMN_MAJOR;
      mycol = rank / NPROW; myrow = rank - mycol * NPROW;
   }
   GRID->iam   = rank;  GRID->myrow = myrow; GRID->mycol  = mycol;
   GRID->nprow = NPROW; GRID->npcol = NPCOL; GRID->nprocs = nprocs;
/*
 * row_ip2   : largest power of two <= nprow;
 * row_hdim  : row_ip2 procs hypercube dim;
 * row_ip2m1 : largest power of two <= nprow-1;
 * row_mask  : row_ip2m1 procs hypercube mask;
 *
 * row_ip2   ：不超过 nprow 的最大 2 的幂；
 * row_hdim  ：对应 row_ip2 个进程时的超立方维度；
 * row_ip2m1 ：不超过 nprow-1 的最大 2 的幂；
 * row_mask  ：对应 row_ip2m1 的超立方掩码。
 */
   hdim = 0; ip2 = 1; k = NPROW;
   while( k > 1 ) { k >>= 1; ip2 <<= 1; hdim++; }
   GRID->row_ip2 = ip2; GRID->row_hdim = hdim; 

   mask = ip2 = 1;    k = NPROW - 1;
   while( k > 1 ) { k >>= 1; ip2 <<= 1; mask <<= 1; mask++; }
   GRID->row_ip2m1 = ip2; GRID->row_mask = mask; 
/*
 * col_ip2   : largest power of two <= npcol;
 * col_hdim  : col_ip2 procs hypercube dim;
 * col_ip2m1 : largest power of two <= npcol-1;
 * col_mask  : col_ip2m1 procs hypercube mask;
 *
 * col_ip2   ：不超过 npcol 的最大 2 的幂；
 * col_hdim  ：对应 col_ip2 个进程时的超立方维度；
 * col_ip2m1 ：不超过 npcol-1 的最大 2 的幂；
 * col_mask  ：对应 col_ip2m1 的超立方掩码。
 */
   hdim = 0; ip2 = 1; k = NPCOL;
   while( k > 1 ) { k >>= 1; ip2 <<= 1; hdim++; }
   GRID->col_ip2 = ip2; GRID->col_hdim = hdim; 

   mask = ip2 = 1;    k = NPCOL - 1;
   while( k > 1 ) { k >>= 1; ip2 <<= 1; mask <<= 1; mask++; }
   GRID->col_ip2m1 = ip2; GRID->col_mask = mask; 
/*
 * All communicator, leave if I am not part of this grid. Creation of the
 * row- and column communicators.
 * 先创建 all communicator；如果当前进程不属于该网格就提前返回；否则
 * 再继续创建按行和按列切分的通信器。
 *
 * HPL communication is layered by communicator:
 * - all_comm is used for test-level collectives and timing aggregation;
 * - row_comm is used for row-wise panel broadcasts and row reductions;
 * - col_comm is used for pivot search, row swaps, U spreading/rolling,
 *   and the triangular solve pipeline.
 *
 * HPL 的通信是分层组织的：
 * - all_comm 用于测试级别的集合操作和计时聚合；
 * - row_comm 用于按进程行执行的 panel 广播与按行归约；
 * - col_comm 用于主元搜索、行交换、U 的 spread/roll，以及回代流水线。
 */
/*
 * First split the incoming communicator COMM into "active grid members"
 * and "non participants".
 * 首先把输入通信器 COMM 切分成“参与当前网格的进程”和“不参与当前网格
 * 的进程”两类。
 *
 * MPI_Comm_split(comm, color, key, newcomm) semantics:
 * MPI_Comm_split(comm, color, key, newcomm) 的语义如下：
 *
 * 1. Processes with the same color are grouped into the same new
 *    communicator.
 * 1. color 相同的进程会被分到同一个新通信器里。
 *
 * 2. Processes passing MPI_UNDEFINED as color do not join any new
 *    communicator, and receive MPI_COMM_NULL in newcomm.
 * 2. 如果某个进程把 color 设为 MPI_UNDEFINED，那么它不会加入任何新
 *    通信器，返回结果 newcomm 会是 MPI_COMM_NULL。
 *
 * 3. Within one color group, key determines the ordering of ranks in the
 *    new communicator.
 * 3. 在同一个 color 分组内部，key 决定新通信器中 rank 的排序顺序。
 *
 * Here HPL uses:
 * 这里 HPL 的具体做法是：
 *
 * - color = 0                   when rank < nprocs
 * - color = MPI_UNDEFINED       when rank >= nprocs
 *
 * This means:
 * 这意味着：
 *
 * - the first nprocs MPI ranks become the active members of the HPL grid;
 * - 前面的 nprocs 个 MPI rank 会成为当前 HPL 网格的有效成员；
 * - all extra ranks launched by mpirun are excluded from the grid;
 * - 超出 nprocs 的其余 rank 会被排除在当前网格之外；
 * - key = rank preserves their original global-rank order inside
 *   GRID->all_comm.
 * - key = rank 则保证这些有效成员在 GRID->all_comm 中保持原始全局
 *   rank 顺序。
 */
   ierr = MPI_Comm_split( COMM, ( rank < nprocs ? 0 : MPI_UNDEFINED ),
                          rank, &(GRID->all_comm) );
/*
 * If this process received MPI_COMM_NULL, it is not part of the current
 * NPROW x NPCOL grid and must return immediately. Such a process remains
 * alive in the MPI job, but it will not participate in any later HPL
 * row/column communication for this grid configuration.
 * 如果当前进程拿到的是 MPI_COMM_NULL，说明它不属于当前这个
 * NPROW x NPCOL 网格，需要立刻返回。这样的进程虽然仍然存在于整个
 * MPI 作业中，但不会参与该网格配置下后续任何 HPL 行/列通信。
 */
   if( GRID->all_comm == MPI_COMM_NULL ) return( ierr );

/*
 * Build the row communicator:
 * 构造按进程行切分的通信器：
 *
 * - color = myrow groups all processes in the same grid row together;
 * - color = myrow 把同一条网格行上的进程分到同一个通信器中；
 * - key   = mycol orders them from left to right by column index.
 * - key   = mycol 则让它们在新通信器中按列号从左到右排列。
 *
 * The resulting GRID->row_comm is later used for panel broadcasts and
 * row-wise reductions.
 * 得到的 GRID->row_comm 后续主要用于 panel 广播和按行归约。
 */
   ierr = MPI_Comm_split( GRID->all_comm, myrow, mycol, &(GRID->row_comm) );
   if( ierr != MPI_SUCCESS ) hplerr = ierr;

/*
 * Build the column communicator:
 * 构造按进程列切分的通信器：
 *
 * - color = mycol groups all processes in the same grid column together;
 * - color = mycol 把同一条网格列上的进程分到同一个通信器中；
 * - key   = myrow orders them from top to bottom by row index.
 * - key   = myrow 则让它们在新通信器中按行号从上到下排列。
 *
 * The resulting GRID->col_comm is later used for pivot search, row swaps,
 * U redistribution, and the triangular-solve pipeline.
 * 得到的 GRID->col_comm 后续主要用于主元搜索、行交换、U 重分发以及
 * 上三角回代流水线。
 */
   ierr = MPI_Comm_split( GRID->all_comm, mycol, myrow, &(GRID->col_comm) );
   if( ierr != MPI_SUCCESS ) hplerr = ierr;

   return( hplerr );
/*
 * End of HPL_grid_init
 */
}
