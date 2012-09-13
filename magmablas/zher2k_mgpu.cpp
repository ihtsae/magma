/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2011

       @precisions normal z -> s d c
       @author Mark Gates
*/
#include "common_magma.h"

/*
    Purpose
    =======
    ZHER2K performs one of the Hermitian rank 2k operations

       C := alpha*A*B**H + conjg( alpha )*B*A**H + beta*C,

    or

       C := alpha*A**H*B + conjg( alpha )*B**H*A + beta*C,

    where alpha and beta are scalars with beta real, C is an n by n
    Hermitian matrix and A and B are n by k matrices in the first case
    and k by n matrices in the second case.

    Arguments
    ==========

    UPLO   - (input) CHARACTER*1.
             On entry, UPLO specifies whether the upper or lower
             triangular part of the array C is to be referenced as
             follows:

                UPLO = 'U' or 'u'   Only the upper triangular part of C
                                    is to be referenced.

                UPLO = 'L' or 'l'   Only the lower triangular part of C
                                    is to be referenced.

    TRANS  - (input) CHARACTER*1.
             On entry, TRANS specifies the operation to be performed as
             follows:

                TRANS = 'N' or 'n'
                  C := alpha*A*B**H + conjg( alpha )*B*A**H + beta*C.

                TRANS = 'C' or 'c'
                  C := alpha*A**H*B + conjg( alpha )*B**H*A + beta*C.

    N      - (input) INTEGER.
             On entry, N specifies the order of the matrix C. N must be
             at least zero.

    K      - (input) INTEGER.
             On entry with TRANS = 'N' or 'n', K specifies the number
             of columns of the matrices A and B, and on entry with
             TRANS = 'C' or 'c', K specifies the number of rows of the
             matrices A and B. K must be at least zero.

    ALPHA  - (input) COMPLEX*16.
             On entry, ALPHA specifies the scalar alpha.

    dA     - (input) COMPLEX*16 array of DIMENSION ( LDA, ka ), where ka is
             k when TRANS = 'N' or 'n', and is n otherwise.
             Before entry with TRANS = 'N' or 'n', the leading n by k
             part of the array A must contain the matrix A, otherwise
             the leading k by n part of the array A must contain the
             matrix A.
             
             [TODO: describe distribution: duplicated on all GPUs.]

    LDA    - (input) INTEGER.
             On entry, LDA specifies the first dimension of A as declared
             in the calling (sub) program. When TRANS = 'N' or 'n'
             then LDA must be at least max( 1, n ), otherwise LDA must
             be at least max( 1, k ).

    dB     - (input) COMPLEX*16 array of DIMENSION ( LDB, kb ), where kb is
             k when TRANS = 'N' or 'n', and is n otherwise.
             Before entry with TRANS = 'N' or 'n', the leading n by k
             part of the array B must contain the matrix B, otherwise
             the leading k by n part of the array B must contain the
             matrix B.
             
             [TODO: describe distribution: duplicated on all GPUs.]

    LDB    - (input) INTEGER.
             On entry, LDB specifies the first dimension of B as declared
             in the calling (sub) program. When TRANS = 'N' or 'n'
             then LDB must be at least max( 1, n ), otherwise LDB must
             be at least max( 1, k ).

    BETA   - (input) DOUBLE PRECISION.
             On entry, BETA specifies the scalar beta.

    dC     - (input/output) COMPLEX*16 array of DIMENSION ( LDC, n ).
             Before entry with UPLO = 'U' or 'u', the leading n by n
             upper triangular part of the array C must contain the upper
             triangular part of the Hermitian matrix and the strictly
             lower triangular part of C is not referenced. On exit, the
             upper triangular part of the array C is overwritten by the
             upper triangular part of the updated matrix.

             Before entry with UPLO = 'L' or 'l', the leading n by n
             lower triangular part of the array C must contain the lower
             triangular part of the Hermitian matrix and the strictly
             upper triangular part of C is not referenced. On exit, the
             lower triangular part of the array C is overwritten by the
             lower triangular part of the updated matrix.

             Note that the imaginary parts of the diagonal elements need
             not be set, they are assumed to be zero, and on exit they
             are set to zero. [TODO: verify]
             
             [TODO: describe distribution: 1D column block-cyclic across GPUs.]

    LDC    - (input) INTEGER.
             On entry, LDC specifies the first dimension of C as declared
             in the calling (sub) program. LDC must be at least
             max( 1, n ).

    OFFSET - (input) INTEGER.
             Row and column to start sub-matrix of dC.
             For example, if offset = 10, then sub-matrix starts at dC(10, 10).
             Offset can be any number, 0 <= offset < ldc.

    NGPU   - (input) INTEGER.
             Number of GPUs over which matrix C is distributed.

    NB     - (input) INTEGER.
             Block size used for distribution of C.

    STREAMS- (input) array of CUDA streams, of dimension NGPU by 20.
             Streams to use for running multiple GEMMs in parallel.
             Only up to NSTREAM streams are used on each GPU.

    NSTREAM- (input) INTEGER.
             Number of streams to use on each device.
*/

extern "C"
void magmablas_zher2k_mgpu2(
    char uplo, char trans, magma_int_t n, magma_int_t k,
    cuDoubleComplex alpha, cuDoubleComplex *dA[], magma_int_t lda,
                           cuDoubleComplex *dB[], magma_int_t ldb,
    double beta,           cuDoubleComplex *dC[], magma_int_t ldc,  magma_int_t offset,
    magma_int_t ngpu, magma_int_t nb, cudaStream_t streams[][20], magma_int_t nstream )
{
    #define dA(dev, i, j) (dA[dev] + (i) + (j)*lda)
    #define dB(dev, i, j) (dB[dev] + (i) + (j)*ldb)
    #define dC(dev, i, j) (dC[dev] + (i) + (j)*ldc)
    
    cuDoubleComplex cbeta = MAGMA_Z_MAKE( beta, 0. );
    
    magma_int_t ib, ioff, iblock, idev, di, s;
    
    magma_device_t cdev;
    magma_queue_t cqueue;
    magma_getdevice( &cdev );
    magmablasGetKernelStream( &cqueue );
    
    // loop over all blocks
    // Faster to have two loops: first does A*B', second does B*A'
    // blockoffset is offset within first block; for subsequent blocks it is 0
    magma_int_t blockoffset = offset % nb;
    for( magma_int_t i = 0; i < n; i += ib ) {
        ib     = min( nb-blockoffset, n-i );  // block size
        ioff   = i + offset;                  // global index in parent matrix
        iblock = (ioff / nb) / ngpu;          // local block id
        idev   = (ioff / nb) % ngpu;          // device with this block
        di     = iblock*nb + blockoffset;     // local index in parent matrix
        
        magma_setdevice( idev );
        s = iblock % nstream;
        magmablasSetKernelStream( streams[ idev ][ s ] );
        
        // C[i:n,i] += A[i:n,0] * B[i,0]'
        //printf( "zgemm  n=%4d, ib=%4d, k=%4d, i=%4d\n", n-i, ib, k, i );
        magma_zgemm( MagmaNoTrans, MagmaConjTrans, n-i, ib, k,
                     alpha, dA(idev,i,0), lda,
                            dB(idev,i,0), ldb,
                     cbeta, dC(idev,ioff,di), ldc );
        blockoffset = 0;
    }
    blockoffset = offset % nb;
    for( magma_int_t i = 0; i < n; i += ib ) {
        ib     = min( nb-blockoffset, n-i );  // block size
        ioff   = i + offset;                  // global index in parent matrix
        iblock = (ioff / nb) / ngpu;          // local block id
        idev   = (ioff / nb) % ngpu;          // device with this block
        di     = iblock*nb + blockoffset;     // local index in parent matrix
        
        magma_setdevice( idev );
        s = iblock % nstream;
        magmablasSetKernelStream( streams[ idev ][ s ] );
        
        // C[i:n,i] += B[i:n,0] * A[i,0]'
        //printf( "zgemm  n=%4d, ib=%4d, k=%4d, i=%4d\n", n-i, ib, k, i );
        magma_zgemm( MagmaNoTrans, MagmaConjTrans, n-i, ib, k,
                     alpha, dB(idev,i,0), ldb,
                            dA(idev,i,0), lda,
                     cbeta, dC(idev,ioff,di), ldc );
        blockoffset = 0;
    }
    
    magma_setdevice( cdev );
    magmablasSetKernelStream( cqueue );
}
