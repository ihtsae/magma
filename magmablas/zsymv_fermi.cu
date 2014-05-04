/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       @date

       @precisions normal z -> c d s

*/
#include "common_magma.h"
#include "commonblas_z.h"

#define PRECISION_z

/* The version for tesla can be found in zsymv_tesla.cu */

#define symv_bs          64
#define thread_x         64
#define thread_y          4
#define bank_shift       33
#define quarter_thread_x 16
#define half_thread_x    32

/*******************************************************************************
 *    Lower case, where n is multiple of block size (symv_bs)
 */

__global__ void
zsymv_kernel_fermi_L_special(
    int n, magmaDoubleComplex alpha,
    const magmaDoubleComplex * __restrict__ A, int lda,
    const magmaDoubleComplex * __restrict__ x, int incx,
    magmaDoubleComplex beta,
    magmaDoubleComplex * __restrict__ y, int incy,
    magmaDoubleComplex * __restrict__ WC)
{
#if (__CUDA_ARCH__ >= 200)
    int tx   = threadIdx.x;
    int ty   = threadIdx.y;
    int blkc = blockIdx.x;

    magmaDoubleComplex res  = MAGMA_Z_ZERO;
    magmaDoubleComplex res_ = MAGMA_Z_ZERO;
    magmaDoubleComplex res1 = MAGMA_Z_ZERO;

    __shared__ magmaDoubleComplex la   [quarter_thread_x][thread_x+2];
    __shared__ magmaDoubleComplex buff [thread_x];
    __shared__ magmaDoubleComplex buff2 [thread_x];

    magmaDoubleComplex tr[4];
    magmaDoubleComplex b[4];

    int break_d   =  thread_x * blkc;
    const int td  = (thread_x * ty) + tx;
    int       tx_ = td % half_thread_x;
    int       ty_ = td / half_thread_x;

    WC +=  break_d + tx;
    x  += (break_d + tx) * incx;
    A  +=  break_d * (lda+1);
    A  += ty_ * lda + tx_;

    if ( ty == 0 ) {
        buff[tx] = x[0];
    } // obtain the vector x store in buff;

    tx = tx_; ty = ty_;

    #pragma unroll
    for(int j=0; j < half_thread_x; j += 8)
        la[0][ bank_shift * (ty_+j) + tx_] = A[ j * lda];
    __syncthreads();

    #pragma unroll
    for(int i=ty_*4; i < (ty_ * 4 + 4); i++) {
        if ( i < tx_ )
            la[0][bank_shift * tx_ + i] = la[0][ bank_shift * i + tx_];
        else
            la[0][bank_shift * tx_ + i] = la[0][ bank_shift * tx_ + i];
    }
    __syncthreads();

    #pragma unroll
    for(int j=0; j < 4; j++)
        res += la[0][bank_shift * tx_ + j + ty_ * 4] * buff[j + ty_ * 4];
    __syncthreads();

    la[0][bank_shift*tx_+ty_] = res;
    __syncthreads();

    if ( ty_ == 0 ) {
        res1 = la[0][tx_*bank_shift+0]+la[0][tx_*bank_shift+1]
             + la[0][tx_*bank_shift+2]+la[0][tx_*bank_shift+3]
             + la[0][tx_*bank_shift+4]+la[0][tx_*bank_shift+5]
             + la[0][tx_*bank_shift+6]+la[0][tx_*bank_shift+7];
    }
    else {
            res1 = MAGMA_Z_ZERO;
    }
    __syncthreads();

    res = MAGMA_Z_ZERO;

    A += half_thread_x + half_thread_x*lda;

    #pragma unroll
    for(int j=0; j < half_thread_x; j += 8)
        la[0][bank_shift*(ty_+j)+tx_] = A[ j * lda];
    __syncthreads();

    #pragma unroll
    for(int i=ty_*4; i < (4+ty_*4); i++) {
        if ( i < tx_ ) {
            la[0][bank_shift*tx_+i] = la[0][bank_shift*i+tx_];
        }
        else
            la[0][bank_shift*tx_+i] = la[0][bank_shift*tx_+i];
    }
    __syncthreads();

    #pragma unroll
    for(int j=0; j < 4; j++)
        res += la[0][bank_shift*tx_+j+ty_*4] * buff[half_thread_x + j + 4 * ty_];
    __syncthreads();
    la[0][bank_shift*tx_+ty_] = res;
    __syncthreads();

    magmaDoubleComplex res2;
    res2 = MAGMA_Z_ZERO;
    if ( ty_ == 1 ) {
        res2 = la[0][tx_*bank_shift+0]+la[0][tx_*bank_shift+1]
             + la[0][tx_*bank_shift+2]+la[0][tx_*bank_shift+3]
             + la[0][tx_*bank_shift+4]+la[0][tx_*bank_shift+5]
             + la[0][tx_*bank_shift+6]+la[0][tx_*bank_shift+7];
    }
    else {
        res2 = MAGMA_Z_ZERO;
    }
    __syncthreads();

    res = MAGMA_Z_ZERO;

    A -= half_thread_x*lda;

    res_ = MAGMA_Z_ZERO;

    #pragma unroll
    for(int j=0; j < half_thread_x; j += 8)
        tr[j/8] = A[ j * lda];

    #pragma unroll
    for(int j=0; j < 4; j++) {
        res += tr[j] * buff[ j*8 + ty_];
        la[0][bank_shift*(ty_+j*8)+tx_] = tr[j];
    }
    __syncthreads();

    #pragma unroll
    for(int j=0; j < 4; j++)
        res_ += la[0][bank_shift*tx_+j+ty_*4] * buff[half_thread_x +j+ty_*4];
    __syncthreads();

    la[0][bank_shift*tx_+ty_] = res;
    __syncthreads();
    if ( ty_ == 1 ) {
        res2 = res2
             + la[0][tx_*bank_shift+0]+la[0][tx_*bank_shift+1]
             + la[0][tx_*bank_shift+2]+la[0][tx_*bank_shift+3]
             + la[0][tx_*bank_shift+4]+la[0][tx_*bank_shift+5]
             + la[0][tx_*bank_shift+6]+la[0][tx_*bank_shift+7];
    }
    else {
            res2 = MAGMA_Z_ZERO;
    }
    __syncthreads();

    la[0][bank_shift*tx_+ty_] = res_;
    __syncthreads();
    if ( ty_ == 0 ) {
        res1 = res1
             + la[0][tx_*bank_shift+0]+la[0][tx_*bank_shift+1]
             + la[0][tx_*bank_shift+2]+la[0][tx_*bank_shift+3]
             + la[0][tx_*bank_shift+4]+la[0][tx_*bank_shift+5]
             + la[0][tx_*bank_shift+6]+la[0][tx_*bank_shift+7];
    }
    else {
            res1 = MAGMA_Z_ZERO;
        }
    A -= half_thread_x;

    __syncthreads();
    tx = threadIdx.x;
    ty = threadIdx.y;

    if ( ty_ == 0 && ty == 0 )
        res = res1;
    else if ( ty_ == 1 && ty == 0 )
        res = res2;
    else {
            res = MAGMA_Z_ZERO;
        }

    A -= ty_ * lda;
    A -= tx_;

    A = A - lda * blkc * thread_x;
    x = x - blkc * thread_x * incx;

    A += 4 * ty * lda;
    A += tx;

    int wc_c = 0;
    int count = 0;

    tx_ = td % quarter_thread_x;
    ty_ = td / quarter_thread_x;

    WC -= tx;
    WC += tx_;

    if ( blkc * thread_x >= thread_x ) {
        #pragma unroll
        for( int i=0; i < thread_x; i += thread_x ) {
            res_ = MAGMA_Z_ZERO;
            count++;
            
            if ( ty == 0 ) {
                buff2[tx] = x[i*incx];
            }
            __syncthreads();

            #pragma unroll
            for( int k=0; k < 4; k++ ) {
                #pragma unroll
                for(int j=0; j < 4; j++)
                    tr[j] = A[j*lda];

                #pragma unroll
                for(int j=0; j < 4; j++) {
                    res += tr[j] * buff2[ quarter_thread_x * k + ty*4 + j];
                    la[j + ty*4][tx] = tr[j] * buff[tx];
                }
                __syncthreads();

                res_ = MAGMA_Z_ZERO;

                #pragma unroll
                for(int j=0; j < 4; j++) {
                    res_ += la[tx_][ty_*4+j];
                }
                b[k] = res_;
                __syncthreads();

                A += lda * quarter_thread_x;
            }

            #pragma unroll
            for(int k=0; k < 4; k++) {
                la[tx_][ty_+quarter_thread_x*k] = b[k];
            }
            __syncthreads();
            if ( ty_ < 4 ) {
                int k = ty_*quarter_thread_x;
                res_ = la[tx_][0+k] + la[tx_][1+k]
                     + la[tx_][2+k] + la[tx_][3+k]
                     + la[tx_][4+k] + la[tx_][5+k]
                     + la[tx_][6+k] + la[tx_][7+k]
                     + la[tx_][8+k] + la[tx_][9+k]
                     + la[tx_][10+k]+ la[tx_][11+k]
                     + la[tx_][12+k]+ la[tx_][13+k]
                     + la[tx_][14+k]+ la[tx_][15+k];
                WC[k + wc_c*lda ] = res_;
            }

            wc_c++;
            __syncthreads();
        }
    }

    for(int i=thread_x; i < (blkc * thread_x); i += thread_x ) {
        res_ = MAGMA_Z_ZERO;
        count++;
        if ( ty == 0 ) {
            buff2[tx] = x[i*incx];
        }
        __syncthreads();

        #pragma unroll
        for( int k=0; k < 4; k++ ) {
            #pragma unroll
            for(int j=0; j < 4; j++)
                tr[j] = A[j*lda];

            #pragma unroll
            for(int j=0; j < 4; j++) {
                res += tr[j] * buff2[ quarter_thread_x*k + ty*4+(j)];
                la[j + ty*4][tx] = tr[j] * buff[tx];
            }
            __syncthreads();

            res_ = MAGMA_Z_ZERO;

            #pragma unroll
            for(int j=0; j < 4; j++)
                res_ += la[tx_][ty_*4+j];

            b[k] = res_;
            __syncthreads();

            A += lda * quarter_thread_x;
        }

        #pragma unroll
        for(int k=0; k < 4; k++) {
            la[tx_][ty_+quarter_thread_x*k] = b[k];
        }
        __syncthreads();
        if ( ty_ < 4 ) {
            int k = ty_*quarter_thread_x;
            res_ = la[tx_][0+k] + la[tx_][1+k]
                 + la[tx_][2+k] + la[tx_][3+k]
                 + la[tx_][4+k] + la[tx_][5+k]
                 + la[tx_][6+k] + la[tx_][7+k]
                 + la[tx_][8+k] + la[tx_][9+k]
                 + la[tx_][10+k]+ la[tx_][11+k]
                 + la[tx_][12+k]+ la[tx_][13+k]
                 + la[tx_][14+k]+ la[tx_][15+k];
            WC[k + wc_c*lda ] = res_;
        }

        wc_c++;
        __syncthreads();
    }

    WC += tx;
    WC -= tx_;

    la[ty][tx] = res;
    __syncthreads();
    if ( ty == 0 ) {
        res = la[0][tx]+ la[1][tx]
            + la[2][tx]+ la[3][tx];
        WC[0+lda*(blkc)] = res;
    }
#endif /* (__CUDA_ARCH__ >= 200) */
}

/**************************************************************
 *    Lower case for generic sizes
 */
__global__ void
zsymv_kernel_fermi_L_generic(
    int n, magmaDoubleComplex alpha,
    const magmaDoubleComplex * __restrict__ A, int lda,
    const magmaDoubleComplex * __restrict__ x, int incx,
    magmaDoubleComplex beta,
    magmaDoubleComplex * __restrict__ y, int incy,
    magmaDoubleComplex * __restrict__ WC,
    int m_mod_thread_x)
{
#if (__CUDA_ARCH__ >= 200)
    int tx   = threadIdx.x;
    int ty   = threadIdx.y;
    int blkc = blockIdx.x;

    magmaDoubleComplex res  = MAGMA_Z_ZERO;
    magmaDoubleComplex res_ = MAGMA_Z_ZERO;
    magmaDoubleComplex res1 = MAGMA_Z_ZERO;

    __shared__ magmaDoubleComplex la   [quarter_thread_x][thread_x+2];
    __shared__ magmaDoubleComplex buff [thread_x];
    __shared__ magmaDoubleComplex buff2[thread_x];

    magmaDoubleComplex tr[4];
    magmaDoubleComplex b[8];

    int break_d   =  thread_x * blkc;
    const int td  = (thread_x * ty) + tx;
    int       tx_ = td % half_thread_x;
    int       ty_ = td / half_thread_x;

    WC +=  break_d + tx;
    x  += (break_d + tx) * incx;
    A  +=  break_d * (lda+1);
    A  += lda * ty_;

    int trackA;
    if ( blkc == ( gridDim.x - 1 ) ) {
        if ( ty == 0 ) {
            if ( tx > m_mod_thread_x ) {
                buff[tx] = MAGMA_Z_ZERO;
            }
            else
                buff[tx] = x[0];
        }
        if ( tx_ > m_mod_thread_x )
            trackA = m_mod_thread_x;
        else
            trackA = tx_;
        A += trackA;
    }
    else {
        if ( ty == 0 ) {
            buff[tx] = x[0];
        }
        trackA = tx_;
        A += trackA;
    }

    // Somehow merging these two if - else creates problem
    // It could be a potential bug -- from synchronization or from cuda or compiler
    if ( blkc == ( gridDim.x - 1 ) ) {
        #pragma unroll
        for(int j=0; j < half_thread_x; j += 8) {
            if ( ( ty_ + j ) > m_mod_thread_x ) {
                la[0][bank_shift*(ty_+j)+tx_] = MAGMA_Z_MAKE( 9999, 0 );
            }
            else
                la[0][bank_shift*(ty_+j)+tx_] = A[ j * lda];
        }
        A -= trackA;
    }
    else {
        #pragma unroll
        for(int j=0; j < half_thread_x; j += 8) {
            la[0][bank_shift*(ty_+j)+tx_] = A[ j * lda];
        }
    }
    tx = tx_;
    ty = ty_;
    __syncthreads();

    #pragma unroll
    for(int i=ty_*4; i < (ty_*4+4); i++) {
        if ( i < tx_ )
            la[0][bank_shift*tx_+i] = la[0][bank_shift*i+tx_];
        else
            la[0][bank_shift*tx_+i] = la[0][bank_shift*tx_+i];
    }
    __syncthreads();

    #pragma unroll
    for(int j=0; j < 4; j++)
        res += la[0][bank_shift*tx_+j+ty_*4] * buff[j+ty_*4];
    __syncthreads();

    la[0][bank_shift*tx_+ty_] = res;
    __syncthreads();
    if ( ty_ == 0 ) {
        res1 = la[0][tx_*bank_shift+0]
             + la[0][tx_*bank_shift+1]
             + la[0][tx_*bank_shift+2]
             + la[0][tx_*bank_shift+3]
             + la[0][tx_*bank_shift+4]
             + la[0][tx_*bank_shift+5]
             + la[0][tx_*bank_shift+6]
             + la[0][tx_*bank_shift+7];
    }
    else {
        res1 = MAGMA_Z_ZERO;
    }
    __syncthreads();

    res = MAGMA_Z_ZERO;

    if ( blkc == ( gridDim.x - 1 ) ) {
        if ( (tx_+half_thread_x) > m_mod_thread_x )
            trackA = m_mod_thread_x;
        else
            trackA = tx_ + half_thread_x;
        A += trackA+half_thread_x*lda;

        #pragma unroll
        for(int j=0; j < half_thread_x; j += 8) {
            if ( ( ty_ + j+half_thread_x ) > m_mod_thread_x ) {
                la[0][bank_shift*(ty_+j)+tx_] = MAGMA_Z_MAKE( 99999, 0 );
            }
            else
                la[0][bank_shift*(ty_+j)+tx_] = A[ j * lda];
        }

        A -= trackA+half_thread_x*lda;
        A += tx_;
        A += half_thread_x + half_thread_x*lda;
    }
    else {
        A += half_thread_x + half_thread_x*lda;

        #pragma unroll
        for(int j=0; j < half_thread_x; j += 8) {
            la[0][bank_shift*(ty_+j)+tx_] = A[ j * lda];
        }
    }

    __syncthreads();
    #pragma unroll
    for(int i=ty_*4; i < (4+ty_*4); i++) {
        if ( i < tx_ ) {
            la[0][bank_shift*tx_+i] = la[0][bank_shift*i+tx_];
        }
        else
            la[0][bank_shift*tx_+i] = la[0][bank_shift*tx_+i];
    }
    __syncthreads();

    #pragma unroll
    for(int j=0; j < 4; j++)
        res += la[0][bank_shift*tx_+j+ty_*4] * buff[half_thread_x + j + 4 * ty_];
    __syncthreads();

    la[0][bank_shift*tx_+ty_] = res;
    __syncthreads();

    magmaDoubleComplex res2;
    res2 = MAGMA_Z_ZERO;
    if ( ty_ == 1 ) {
        res2 = la[0][tx_*bank_shift+0]
             + la[0][tx_*bank_shift+1]
             + la[0][tx_*bank_shift+2]
             + la[0][tx_*bank_shift+3]
             + la[0][tx_*bank_shift+4]
             + la[0][tx_*bank_shift+5]
             + la[0][tx_*bank_shift+6]
             + la[0][tx_*bank_shift+7];
    }
    else {
        res2 = MAGMA_Z_ZERO;
    }
    __syncthreads();

    res  = MAGMA_Z_ZERO;
    res_ = MAGMA_Z_ZERO;

    A -= half_thread_x*lda;
    if ( blkc == ( gridDim.x - 1 ) ) {
        A -= tx_;
        if ( tx_ > m_mod_thread_x )
            trackA = m_mod_thread_x;
        else
            trackA = tx_;
        A += trackA;

        #pragma unroll
        for(int j=0; j < half_thread_x; j += 8)
            if ( ( ty_ + j ) > m_mod_thread_x ) {
                tr[j/8] = MAGMA_Z_MAKE( 99999, 0 );
            }
            else
                tr[j/8] = A[ j * lda];
        A -= trackA;
        A += tx_;
    }
    else {
        #pragma unroll
        for(int j=0; j < half_thread_x; j += 8)
            tr[j/8] = A[ j * lda];
    }
    __syncthreads();

    #pragma unroll
    for(int j=0; j < 4; j++) {
        res += tr[j] * buff[ j*8 + ty_];
        la[0][bank_shift*(ty_+j*8)+tx_] = tr[j];
    }
    __syncthreads();

    #pragma unroll
    for(int j=0; j < 4; j++)
        res_ += la[0][bank_shift*tx_+j+ty_*4] * buff[half_thread_x +j+ty_*4];
    __syncthreads();

    la[0][bank_shift*tx_+ty_] = res;
    __syncthreads();
    if ( ty_ == 1 ) {
        res2 = res2
             + la[0][tx_*bank_shift+0]
             + la[0][tx_*bank_shift+1]
             + la[0][tx_*bank_shift+2]
             + la[0][tx_*bank_shift+3]
             + la[0][tx_*bank_shift+4]
             + la[0][tx_*bank_shift+5]
             + la[0][tx_*bank_shift+6]
             + la[0][tx_*bank_shift+7];
    }
    else {
        res2 = MAGMA_Z_ZERO;
    }
    __syncthreads();

    la[0][bank_shift*tx_+ty_] = res_;
    __syncthreads();

    if ( ty_ == 0 ) {
        res1 = res1
             + la[0][tx_*bank_shift+0]
             + la[0][tx_*bank_shift+1]
             + la[0][tx_*bank_shift+2]
             + la[0][tx_*bank_shift+3]
             + la[0][tx_*bank_shift+4]
             + la[0][tx_*bank_shift+5]
             + la[0][tx_*bank_shift+6]
             + la[0][tx_*bank_shift+7];
    }
    else {
        res1 = MAGMA_Z_ZERO;
    }
    A -= half_thread_x;

    __syncthreads();
    tx = threadIdx.x;
    ty = threadIdx.y;

    if ( ty_ == 0 && ty == 0 )
        res = res1;
    else if ( ty_ == 1 && ty == 0 )
        res = res2;
    else {
        res = MAGMA_Z_ZERO;
    }

    A -= ty_ * lda;
    A -= tx_;

    A = A - lda*break_d;
    x = x - break_d*incx;

    A += 4 * ty * lda;

    if ( blkc == ( gridDim.x - 1 ) ) {
        if ( tx <= m_mod_thread_x )
            A += tx;
        else
            A += m_mod_thread_x;
    }
    else{
        A += tx;
    }

    int wc_c = 0;
    int count = 0;

    tx_ = td % quarter_thread_x;
    ty_ = td / quarter_thread_x;

    WC -= tx;
    WC += tx_;

    #pragma unroll
    for(int j=0; j < 4; j++)
        b[j] = buff[ty_*4+j];

    if ( break_d > 0 )
        #pragma unroll
        for( int i=0; i < thread_x; i += thread_x ) {
            res_ = MAGMA_Z_ZERO;
            count++;
            if ( ty == 0 ) {
                buff2[tx] = x[i*incx];
            }
            __syncthreads();

            #pragma unroll
            for( int k=0; k < 4; k++ ) {
                #pragma unroll
                for(int j=0; j < 4; j++)
                    tr[j] = A[j*lda];

                #pragma unroll
                for(int j=0; j < 4; j++) {
                    res += tr[j]*buff2[quarter_thread_x*k + ty*4+(j)];
                    la[( (j)+ty*4)][tx] = tr[j];
                }
                __syncthreads();

                res_ = MAGMA_Z_ZERO;

                #pragma unroll
                for(int j=0; j < 4; j++)
                    res_ += la[tx_][ty_*4+j] * b[j];
                b[4+k] = res_;
                __syncthreads();
                A += lda * quarter_thread_x;
            }

            #pragma unroll
            for(int k=0; k < 4; k++) {
                la[tx_][ty_+quarter_thread_x*k] = b[4+k];
            }
            __syncthreads();

            if ( ty_ < 4 ) {
                int k = ty_*quarter_thread_x;
                res_ = la[tx_][0+k] + la[tx_][1+k]
                     + la[tx_][2+k] + la[tx_][3+k]
                     + la[tx_][4+k] + la[tx_][5+k]
                     + la[tx_][6+k] + la[tx_][7+k]
                     + la[tx_][8+k] + la[tx_][9+k]
                     + la[tx_][10+k]+ la[tx_][11+k]
                     + la[tx_][12+k]+ la[tx_][13+k]
                     + la[tx_][14+k]+ la[tx_][15+k];
                WC[k + wc_c*lda ] = res_;
            }
            wc_c++;
            __syncthreads();
        }

    for( int i=thread_x; i < break_d; i += thread_x ) {
        res_ = MAGMA_Z_ZERO;
        count++;
        if ( ty == 0 )
            buff2[tx] = x[i*incx];
        __syncthreads();

        #pragma unroll
        for( int k=0; k < 4; k++ ) {
            #pragma unroll
            for(int j=0; j < 4; j++)
                tr[j] = A[j*lda];
            #pragma unroll
            for(int j=0; j < 4; j++) {
                res += tr[j]*buff2[quarter_thread_x*k + ty*4+(j)];
                la[( (j)+ty*4)][tx] = tr[j];
            }
            __syncthreads();

            res_ = MAGMA_Z_ZERO;

            #pragma unroll
            for(int j=0; j < 4; j++)
                res_ += la[tx_][ty_*4+j] * b[j];
            b[4+k] = res_;
            __syncthreads();
            A += lda * quarter_thread_x;
        }

        #pragma unroll
        for(int k=0; k < 4; k++) {
            la[tx_][ty_+quarter_thread_x*k] = b[4+k];
        }
        __syncthreads();

        if ( ty_ < 4 ) {
            int k = ty_*quarter_thread_x;
            res_ = la[tx_][0+k] + la[tx_][1+k]
                 + la[tx_][2+k] + la[tx_][3+k]
                 + la[tx_][4+k] + la[tx_][5+k]
                 + la[tx_][6+k] + la[tx_][7+k]
                 + la[tx_][8+k] + la[tx_][9+k]
                 + la[tx_][10+k]+ la[tx_][11+k]
                 + la[tx_][12+k]+ la[tx_][13+k]
                 + la[tx_][14+k]+ la[tx_][15+k];
            WC[k + wc_c*lda ] = res_;
        }
        wc_c++;
        __syncthreads();
    }

    WC += tx;
    WC -= tx_;
    la[ty][tx] = res;
    __syncthreads();

    if ( ty == 0 ) {
        res=la[0][tx]+ la[1][tx]+ la[2][tx]+ la[3][tx];
        WC[0+lda*(blkc)] = res;
    }
#endif /* (__CUDA_ARCH__ >= 200) */
}

__global__ void
zsymv_kernel_fermi_L_update(
    int n, magmaDoubleComplex alpha,
    const magmaDoubleComplex * __restrict__ A, int lda,
    const magmaDoubleComplex * __restrict__ x, int incx,
    magmaDoubleComplex beta,
    magmaDoubleComplex * __restrict__ y, int incy,
    magmaDoubleComplex * __restrict__ WC )
{
#if (__CUDA_ARCH__ >= 200)
    int i;
    int tx  = threadIdx.x;
    int ind = blockIdx.x * thread_x + tx;
    magmaDoubleComplex Ca;

    Ca = MAGMA_Z_ZERO;
    WC += ind + lda * blockIdx.x;

    for(i = blockIdx.x*thread_x; i < n; i += thread_x) {
        Ca += WC[0];
        WC += thread_x;
    }
    if ( ind < n )
        y[ind * incy] = beta * y[ind * incy] + alpha * Ca;
#endif /* (__CUDA_ARCH__ >= 200) */
}


extern "C"
void magmablas_zsymv_fermi_L(
    magma_int_t n, magmaDoubleComplex alpha,
    const magmaDoubleComplex *A, magma_int_t lda,
    const magmaDoubleComplex *x, magma_int_t incx,
    magmaDoubleComplex beta,
    magmaDoubleComplex *y, magma_int_t incy,
    magmaDoubleComplex *dwork)
{
    magma_int_t blocks = (n - 1)/symv_bs + 1;
    dim3 grid(blocks, 1, 1);
    dim3 threads(thread_x, thread_y, 1);
    dim3 threads_u(symv_bs, 1, 1);

    /*
     * If matrix size is multiple of symv_bs, we use a specific code.
     * otherwise, we call the generic case.
     */
    if ( n % symv_bs == 0 ) {
        zsymv_kernel_fermi_L_special<<< grid, threads, 0, magma_stream >>>
            (n, alpha, A, lda, x, incx, beta, y, incy, dwork);
    }
    else{
        magma_int_t m_mod_thread_x = (n % symv_bs) - 1;
        zsymv_kernel_fermi_L_generic<<< grid, threads, 0, magma_stream >>>
            (n, alpha, A, lda, x, incx, beta, y, incy, dwork, m_mod_thread_x);
    }

    zsymv_kernel_fermi_L_update<<< grid, threads_u, 0, magma_stream >>>
        (n, alpha, A, lda, x, incx, beta, y, incy, dwork);
}

/**
    Purpose
    -------
    magmablas_zsymv performs the matrix-vector operation:

        y := alpha*A*x + beta*y,

    where alpha and beta are scalars, x and y are n element vectors and
    A is an n by n symmetric matrix.

    Arguments
    ----------
    @param[in]
    uplo    magma_uplo_t.
            On entry, UPLO specifies whether the upper or lower
            triangular part of the array A is to be referenced as
            follows:
      -     = MagmaUpper:  Only the upper triangular part of A is to be referenced.
      -     = MagmaLower:  Only the lower triangular part of A is to be referenced.

    @param[in]
    n       INTEGER.
            On entry, N specifies the order of the matrix A.
            N must be at least zero.

    @param[in]
    alpha   COMPLEX*16.
            On entry, ALPHA specifies the scalar alpha.

    @param[in]
    A       COMPLEX*16 array of DIMENSION ( LDA, n ).
            Before entry with UPLO = MagmaUpper, the leading n by n
            upper triangular part of the array A must contain the upper
            triangular part of the hermitian matrix and the strictly
            lower triangular part of A is not referenced.
            Before entry with UPLO = MagmaLower, the leading n by n
            lower triangular part of the array A must contain the lower
            triangular part of the hermitian matrix and the strictly
            upper triangular part of A is not referenced.
            Note that the imaginary parts of the diagonal elements need
            not be set and are assumed to be zero.

    @param[in]
    lda     INTEGER.
            On entry, LDA specifies the first dimension of A as declared
            in the calling (sub) program. LDA must be at least
            max( 1, n ).
            It is recommended that lda is multiple of 16. Otherwise
            performance would be deteriorated as the memory accesses
            would not be fully coalescent.

    @param[in]
    x       COMPLEX*16 array of dimension at least
            ( 1 + ( n - 1 )*abs( INCX ) ).
            Before entry, the incremented array X must contain the n
            element vector x.

    @param[in]
    incx    INTEGER.
            On entry, INCX specifies the increment for the elements of
            X. INCX must not be zero.

    @param[in]
    beta    COMPLEX*16.
            On entry, BETA specifies the scalar beta. When BETA is
            supplied as zero then Y need not be set on input.

    @param[in,out]
    y       COMPLEX*16 array of dimension at least
            ( 1 + ( n - 1 )*abs( INCY ) ).
            Before entry, the incremented array Y must contain the n
            element vector y. On exit, Y is overwritten by the updated
            vector y.

    @param[in]
    incy    INTEGER.
            On entry, INCY specifies the increment for the elements of
            Y. INCY must not be zero.

    @ingroup magma_zblas2
    ********************************************************************/
extern "C"
magma_int_t
magmablas_zsymv(
    magma_uplo_t uplo, magma_int_t n,
    magmaDoubleComplex alpha,
    const magmaDoubleComplex *A, magma_int_t lda,
    const magmaDoubleComplex *x, magma_int_t incx,
    magmaDoubleComplex beta,
    magmaDoubleComplex *y, magma_int_t incy)
{
    magma_int_t arch = magma_getdevice_arch();
    if ( arch < 200 ) {
        // --------------------
        // call CUDA ARCH 1.x version
        // magmablas for [sdc] precisions, cublas for [z] precisions.
        // NOTE: [cz]symv are not implemented in cublas v1, but are in cublas v2.
        #if defined(PRECISION_z) || defined(PRECISION_c)
        fprintf(stderr, "%s: %s\n", __func__, "complex case not implemented");
        //cublasZsymv( lapacke_uplo_const(uplo), n, alpha, A, lda, x, incx, beta, y, incy );
        #else
        magmablas_zsymv_tesla( uplo, n, alpha, A, lda, x, incx, beta, y, incy );
        #endif
        return MAGMA_SUCCESS;
    }
    
    // --------------------
    // CUDA ARCH 2.x (Fermi) version
    int upper = (uplo == MagmaUpper);

    /*
     * Test the input parameters.
     */
    if ( (! upper) && (uplo != MagmaLower) ) {
        return -1;
    } else if ( n < 0 ) {
        return -2;
    } else if ( lda < max(1,n) ) {
        return -5;
    } else if ( incx == 0 ) {
        return -7;
    } else if ( incy == 0 ) {
        return -10;
    }

    /*
     * Quick return if possible.
     */
    if ( (n == 0) || ( MAGMA_Z_EQUAL(alpha, MAGMA_Z_ZERO) && MAGMA_Z_EQUAL(beta, MAGMA_Z_ONE) ) )
        return MAGMA_SUCCESS;

    /* TODO: Upper case is not implemented in MAGMA */
    /* NOTE: [cz]symv are not implemented in cublas v1, but are in cublas v2. */
    if ( upper ) {
        #if defined(PRECISION_z) || defined(PRECISION_c)
        fprintf(stderr, "%s: %s\n", __func__, "Upper case not implemented");
        //cublasZsymv( lapacke_uplo_const(uplo), n, alpha, A, lda, x, incx, beta, y, incy );
        #else
        magma_zsymv( uplo, n, alpha, A, lda, x, incx, beta, y, incy);
        #endif
    }
    else {
        magmaDoubleComplex *dwork;
        magma_int_t blocks = (n - 1)/thread_x + 1;
        magma_int_t lwork  = lda * (blocks + 1);

        // TODO deal with error
        magma_zmalloc( &dwork, lwork );

        magmablas_zsymv_work( uplo, n, alpha, A, lda, x, incx, beta, y, incy, dwork, lwork );

        magma_free( dwork );
    }
    return MAGMA_SUCCESS;
}


/*************************************************************************

    Purpose
    -------

    magmablas_zsymv_work performs the matrix-vector operation:

        y := alpha*A*x + beta*y,

    where alpha and beta are scalars, x and y are n element vectors and
    A is an n by n hermitian matrix.

    the interface of magmablas_zsymv_work is different from magmablas_zsymv in
    the last argument dwork

    MAGMA implements zsymv through two steps:
    1)  perform the multiplication in each thread blocks and put the intermediate value
        in a space of device memory which we call working space. dwork is the working space
    2)  sum the intermediate values and store the final result in y.
    
    The size of dwork is
        lda * ceil(n/thread_x)
    where thread_x = 64
    
    magamblas_zsymv_work requires users to provide a working space, while magmablas_zsymv is
    a wrapper routine allocating the working space inside the routine
    and provides the same interface with cublas.
    
    If users need to call zsymv frequently, we suggest using magmablas_zsymv_work instead of magmablas_zsymv,
    as the overhead of allocating and free device memory in magmablas_zsymv would hurt performance.
    Our tests show that this penalty is about 10 Gflop/s when matrix size is around 10000.
    
*/

extern "C"
magma_int_t
magmablas_zsymv_work(
    magma_uplo_t uplo, magma_int_t n,
    magmaDoubleComplex alpha,
    const magmaDoubleComplex *A, magma_int_t lda,
    const magmaDoubleComplex *x, magma_int_t incx,
    magmaDoubleComplex beta,
    magmaDoubleComplex *y, magma_int_t incy,
    magmaDoubleComplex *dwork, magma_int_t lwork)
{
    magma_int_t arch = magma_getdevice_arch();
    if ( arch < 200 ) {
        // --------------------
        // call CUDA ARCH 1.x version
        // magmablas for [sdc] precisions, cublas for [z] precisions.
        // NOTE: [z]symv is not implemented in cublas v1, but is in cublas v2.
        #if defined(PRECISION_z)
        fprintf(stderr, "%s: %s\n", __func__, "complex case not implemented");
        //cublasZsymv( lapacke_uplo_const(uplo), n, alpha, A, lda, x, incx, beta, y, incy );
        #else
        magmablas_zsymv_tesla_work( uplo, n, alpha, A, lda, x, incx, beta, y, incy, dwork, lwork );
        #endif
        return MAGMA_SUCCESS;
    }
    
    // --------------------
    // CUDA ARCH 2.x (Fermi) version
    int upper = (uplo == MagmaUpper);

    /*
     * Test the input parameters.
     */
    if ( (! upper) && (uplo != MagmaLower) ) {
        return -1;
    } else if ( n < 0 ) {
        return -2;
    } else if ( lda < max(1,n) ) {
        return -5;
    } else if ( incx == 0 ) {
        return -7;
    } else if ( incy == 0 ) {
        return -10;
    }

    /*
     * Quick return if possible.
     */
    if ( (n == 0) || ( MAGMA_Z_EQUAL(alpha, MAGMA_Z_ZERO) && MAGMA_Z_EQUAL(beta, MAGMA_Z_ONE) ) )
        return MAGMA_SUCCESS;
    
    /* TODO: Upper case is not implemented in MAGMA */
    /* NOTE: [cz]symv are not implemented in cublas v1, but are in cublas v2. */
    if ( upper ) {
        #if defined(PRECISION_z) || defined(PRECISION_c)
        fprintf(stderr, "%s: %s\n", __func__, "Upper case not implemented");
        return MAGMA_ERR_NOT_SUPPORTED;
        #else
        magma_zsymv( uplo, n, alpha, A, lda, x, incx, beta, y, incy);
        #endif
    }
    else {
        /* TODO check lwork size! */
        magmablas_zsymv_fermi_L(n, alpha, A, lda, x, incx, beta, y, incy, dwork);
    }
    return MAGMA_SUCCESS;
}
