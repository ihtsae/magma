/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       @date

       @precisions normal z

       @author Jakub Kurzak
       @author Stan Tomov
       @author Mark Gates
       @author Azzam Haidar
       @author Ahmad Abdelfattah
       
*/

#include "common_magma.h"
#define PRECISION_z

#include "gemm_template_kernel_batched.cuh"
#include "gemm_config/zgemm_param_nn.h"
#include "gemm_config/zgemm_param_nt.h"
#include "gemm_config/zgemm_param_tn.h"
#include "gemm_config/zgemm_param_tt.h"
#define version(s,v) s ## _V_ ## v

/**
    Purpose
    -------
    ZGEMM performs one of the matrix-matrix operations
    
        C = alpha*op( A )*op( B ) + beta*C,
    
    where op( X ) is one of
    
        op( X ) = X   or   op( X ) = X**T   or   op( X ) = X**H,
    
    alpha and beta are scalars, and A, B and C are matrices, with
    op( A ) an m by k matrix, op( B ) a k by n matrix and C an m by n matrix.
    
    Parameters
    ----------
    @param[in]
    transA  CHARACTER*1.
            On entry, transA specifies the form of op( A ) to be used in
            the matrix multiplication as follows:
      -     = 'N':  op( A ) = A.
      -     = 'T':  op( A ) = A**T.
      -     = 'C':  op( A ) = A**H.
    
    @param[in]
    transB  CHARACTER*1.
            On entry, transB specifies the form of op( B ) to be used in
            the matrix multiplication as follows:
      -     = 'N':  op( B ) = B.
      -     = 'T':  op( B ) = B**T.
      -     = 'C':  op( B ) = B**H.
    
    @param[in]
    m       INTEGER.
            On entry,  M  specifies  the number  of rows  of the  matrix
            op( dA )  and of the  matrix dC.  M  must  be at least  zero.
    
    @param[in]
    n       INTEGER.
            On entry,  N  specifies the number  of columns of the matrix
            op( dB ) and the number of columns of the matrix dC. N must be
            at least zero.
    
    @param[in]
    k       INTEGER.
            On entry,  K  specifies  the number of columns of the matrix
            op( dA ) and the number of rows of the matrix op( dB ). K must
            be at least  zero.
    
    @param[in]
    alpha   COMPLEX_16
            On entry, ALPHA specifies the scalar alpha.
    
    @param[in]
    dA      COMPLEX_16 array of DIMENSION ( LDA, ka ), where ka is
            k  when  transA = MagmaNoTrans,  and is  m  otherwise.
            Before entry with  transA = MagmaNoTrans,  the leading  m by k
            part of the array dA must contain the matrix dA, otherwise
            the leading  k by m  part of the array dA must contain  the
            matrix dA.
    
    @param[in]
    ldda    INTEGER.
            On entry, LDA specifies the first dimension of A as declared
            in the calling (sub) program. When  transA = MagmaNoTrans then
            LDA must be at least  max( 1, m ), otherwise  LDA must be at
            least  max( 1, k ).
    
    @param[in]
    dB      COMPLEX_16 array of DIMENSION ( LDB, kb ), where kb is
            n  when  transB = MagmaNoTrans,  and is  k  otherwise.
            Before entry with  transB = MagmaNoTrans,  the leading  k by n
            part of the array dB must contain the matrix dB, otherwise
            the leading  n by k  part of the array dB must contain  the
            matrix dB.
    
    @param[in]
    lddb    INTEGER.
            On entry, LDB specifies the first dimension of dB as declared
            in the calling (sub) program. When  transB = MagmaNoTrans then
            LDB must be at least  max( 1, k ), otherwise  LDB must be at
            least  max( 1, n ).
    
    @param[in]
    beta    COMPLEX_16.
            On entry,  BETA  specifies the scalar  beta.  When  BETA  is
            supplied as zero then dC need not be set on input.
    
    @param[in,out]
    dC      COMPLEX_16 array of DIMENSION ( LDC, n ).
            Before entry, the leading  m by n  part of the array  dC must
            contain the matrix  dC,  except when  beta  is zero, in which
            case dC need not be set on entry.
            On exit, the array  dC  is overwritten by the  m by n  matrix
            ( alpha*op( dA )*op( dB ) + beta*dC ).
    
    @param[in]
    lddc    INTEGER.
            On entry, LDC specifies the first dimension of dC as declared
            in  the  calling  (sub)  program.   LDC  must  be  at  least
            max( 1, m ).

    @ingroup magma_zblas3
    ********************************************************************/
void
magmablas_zgemm_batched(
    magma_trans_t transA, magma_trans_t transB, 
    magma_int_t m, magma_int_t n, magma_int_t k,
    magmaDoubleComplex alpha,
    magmaDoubleComplex const * const * dA_array, magma_int_t ldda,
    magmaDoubleComplex const * const * dB_array, magma_int_t lddb,
    magmaDoubleComplex beta,
    magmaDoubleComplex **dC_array, magma_int_t lddc, 
    magma_int_t batchCount, magma_queue_t queue )
{
    magma_int_t info = 0;
    if      ( transA != MagmaNoTrans && transA != MagmaTrans && transA != MagmaConjTrans )
        info = -1;
    else if ( transB != MagmaNoTrans && transB != MagmaTrans && transB != MagmaConjTrans )
        info = -2;
    else if ( m < 0 )
        info = -3;
    else if ( n < 0 )
        info = -4;
    else if ( k < 0 )
        info = -5;
    else if ( transA == MagmaNoTrans ? ldda < m : ldda < k )
        info = -8;
    else if ( transB == MagmaNoTrans ? lddb < k : lddb < n )
        info = -10;
    else if ( lddc < m )
        info = -13;
    
    if (info != 0) {
        magma_xerbla( __func__, -(info) );
        return;  //info;
    }
    
    magma_int_t arch = magma_getdevice_arch();
    if ( arch < 200  ) {
        printf("arch < 200 not supported \n"); // TODO call cublas
        return;
    }
    
    if ( m <= 0 || n <= 0 || k <= 0 )
        return;

    magma_int_t shape;
    if      (transA == MagmaNoTrans   && transB == MagmaNoTrans)   {shape = 0;} // nn
    else if (transA == MagmaNoTrans   && transB == MagmaTrans)     {shape = 1;} // nt
    else if (transA == MagmaNoTrans   && transB == MagmaConjTrans) {shape = 2;} // nc
    else if (transA == MagmaTrans     && transB == MagmaNoTrans)   {shape = 3;} // tn
    else if (transA == MagmaTrans     && transB == MagmaTrans)     {shape = 4;} // tt
    else if (transA == MagmaTrans     && transB == MagmaConjTrans) {shape = 5;} // tc
    else if (transA == MagmaConjTrans && transB == MagmaNoTrans)   {shape = 6;} // cn
    else if (transA == MagmaConjTrans && transB == MagmaTrans)     {shape = 7;} // ct
    else if (transA == MagmaConjTrans && transB == MagmaConjTrans) {shape = 8;} // cc
    
    //TODO: probably the texture init code should be placed here

    size_t offsetA = 0;
    size_t offsetB = 0;
    offsetA = offsetA/sizeof(magmaDoubleComplex);
    offsetB = offsetB/sizeof(magmaDoubleComplex);
    
    switch(shape)
    {
        case 0: // nn
            {
                gemm_template_batched_nn<magmaDoubleComplex, version(NN,18), 0, 0>
                (m, n, k, dA_array, ldda, dB_array, lddb, dC_array, lddc, alpha, beta, offsetA, offsetB, batchCount, queue);
            }
            break;
        case 1: // nt
            {
                if(k <= 8)
                {
                    // version 58
                    gemm_template_batched_nt<magmaDoubleComplex, version(NT,58), 0, 0>
                    (m, n, k, dA_array, ldda, dB_array, lddb, dC_array, lddc, alpha, beta, offsetA, offsetB, batchCount, queue);
                }
                else
                {
                    // version 29
                    gemm_template_batched_nt<magmaDoubleComplex, version(NT,29), 0, 0>
                    (m, n, k, dA_array, ldda, dB_array, lddb, dC_array, lddc, alpha, beta, offsetA, offsetB, batchCount, queue);
                }
            }
            break;
        case 2: // nc
            {
                if(k <= 8)
                {
                    // version 58
                    gemm_template_batched_nt<magmaDoubleComplex, version(NT,58), 0, 1>
                    (m, n, k, dA_array, ldda, dB_array, lddb, dC_array, lddc, alpha, beta, offsetA, offsetB, batchCount, queue);
                }
                else
                {
                    // version 29
                    gemm_template_batched_nt<magmaDoubleComplex, version(NT,29), 0, 1>
                    (m, n, k, dA_array, ldda, dB_array, lddb, dC_array, lddc, alpha, beta, offsetA, offsetB, batchCount, queue);
                }
            }
            break;
        case 3: // tn
            {
                // version 72
                gemm_template_batched_tn<magmaDoubleComplex, version(TN,72), 0, 0>
                (m, n, k, dA_array, ldda, dB_array, lddb, dC_array, lddc, alpha, beta, offsetA, offsetB, batchCount, queue);
            }
            break;
        case 6: // cn
            {
                // version 72
                gemm_template_batched_tn<magmaDoubleComplex, version(TN,72), 1, 0>
                (m, n, k, dA_array, ldda, dB_array, lddb, dC_array, lddc, alpha, beta, offsetA, offsetB, batchCount, queue);
            }
            break;
        case 4: // tt
            {
                // version 13
                gemm_template_batched_tt<magmaDoubleComplex, version(TT,13), 0, 0>
                (m, n, k, dA_array, ldda, dB_array, lddb, dC_array, lddc, alpha, beta, offsetA, offsetB, batchCount, queue);
            }
            break;
        case 5: // tc
            {
                // version 13
                gemm_template_batched_tt<magmaDoubleComplex, version(TT,13), 0, 1>
                (m, n, k, dA_array, ldda, dB_array, lddb, dC_array, lddc, alpha, beta, offsetA, offsetB, batchCount, queue);
            }
            break;
        case 7: // ct
            {
                // version 13
                gemm_template_batched_tt<magmaDoubleComplex, version(TT,13), 1, 0>
                (m, n, k, dA_array, ldda, dB_array, lddb, dC_array, lddc, alpha, beta, offsetA, offsetB, batchCount, queue);
            }
            break;
        case 8: // cc
            {
                // version 13
                gemm_template_batched_tt<magmaDoubleComplex, version(TT,13), 1, 1>
                (m, n, k, dA_array, ldda, dB_array, lddb, dC_array, lddc, alpha, beta, offsetA, offsetB, batchCount, queue);
            }
            break;
        default:; // propose something
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////