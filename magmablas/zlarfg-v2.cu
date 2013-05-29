/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2011

       @precisions normal z -> s d c

*/
#include "common_magma.h"

// 512 is maximum number of threads for CUDA capability 1.x
#define BLOCK_SIZE 512

#define PRECISION_z

__global__
void magma_zlarfg_gpu_kernel( int n, magmaDoubleComplex* dx0, magmaDoubleComplex* dx, 
                              magmaDoubleComplex *dtau, double *dxnorm )
{
    const int i = threadIdx.x;
    const int j = i + BLOCK_SIZE * blockIdx.x;
    __shared__ magmaDoubleComplex scale;
    __shared__ double xnorm;    
  
    magmaDoubleComplex dxi;

#if (defined(PRECISION_s) || defined(PRECISION_d))
    if( n <= 1 ) {
#else
    if( n <= 0 ) {
#endif
        *dtau = MAGMA_Z_ZERO;
        return;
    }

    if ( j < n-1)
        dxi = dx[j];
  
    if ( i == 0 ) {
        xnorm = *dxnorm;
        if ( xnorm == 0 ) {
            *dtau = MAGMA_Z_ZERO;
        }
        else {

#if (defined(PRECISION_s) || defined(PRECISION_d))
            double alpha = *dx0;

            // no need to compute the norm as it is passed as input
            double beta  = xnorm; // sqrt( alpha*alpha + xnorm*xnorm );
            beta  = -copysign( beta, alpha );
 
            // todo: deal with badly scaled vectors (see lapack's larfg)
            *dtau = (beta - alpha) / beta;
            if (blockIdx.x == gridDim.x-1)
            *dx0  = beta;

            scale = 1. / (alpha - beta);
#else
            magmaDoubleComplex alpha = *dx0;
            double alphar =  MAGMA_Z_REAL(alpha), alphai = MAGMA_Z_IMAG(alpha);

            // no need to compute the norm as it is passed as input
            double beta  = xnorm; // sqrt( alphar*alphar + alphai*alphai + xnorm*xnorm );
            beta  = -copysign( beta, alphar );

            // todo: deal with badly scaled vectors (see lapack's larfg)
            *dtau = MAGMA_Z_MAKE((beta - alphar)/beta, -alphai/beta);
            if (blockIdx.x == gridDim.x-1)
            *dx0  = MAGMA_Z_MAKE(beta, 0.);
            
            alpha = MAGMA_Z_MAKE( MAGMA_Z_REAL(alpha) - beta, MAGMA_Z_IMAG(alpha));
            scale = MAGMA_Z_DIV( MAGMA_Z_ONE, alpha);
#endif
        }
    }

    // scale x
    __syncthreads();
    if ( xnorm != 0 && j < n-1)
        dx[j] = MAGMA_Z_MUL(dxi, scale);
}

__global__
void magma_zlarfg2_gpu_kernel( int n, magmaDoubleComplex* dx0, magmaDoubleComplex* dx,
                              magmaDoubleComplex *dtau, double *dxnorm, magmaDoubleComplex* dAkk)
{
    const int i = threadIdx.x;
    const int j = i + BLOCK_SIZE * blockIdx.x;
    __shared__ magmaDoubleComplex scale;
    __shared__ double xnorm;

    magmaDoubleComplex dxi;

/*
#if (defined(PRECISION_s) || defined(PRECISION_d))
    if( n <= 1 ) {
#else
    if( n <= 0 ) {
#endif
        *dtau = MAGMA_Z_ZERO;
        return;
    }
*/
    if ( j < n-1)
        dxi = dx[j];

    if ( i == 0 ) {
        xnorm = *dxnorm;
        if ( xnorm == 0 ) {
            *dtau = MAGMA_Z_ZERO;
        }
        else {

#if (defined(PRECISION_s) || defined(PRECISION_d))
            double alpha = *dx0;

            // no need to compute the norm as it is passed as input
            double beta  = xnorm; // sqrt( alpha*alpha + xnorm*xnorm );
            beta  = -copysign( beta, alpha );

            // todo: deal with badly scaled vectors (see lapack's larfg)
            *dtau = (beta - alpha) / beta;
            //if (blockIdx.x == gridDim.x-1)
            //*dx0  = beta;
            *dAkk  = beta;

            scale = 1. / (alpha - beta);
#else
            magmaDoubleComplex alpha = *dx0;
            double alphar =  MAGMA_Z_REAL(alpha), alphai = MAGMA_Z_IMAG(alpha);

            // no need to compute the norm as it is passed as input
            double beta  = xnorm; // sqrt( alphar*alphar + alphai*alphai + xnorm*xnorm );
            beta  = -copysign( beta, alphar );

            // todo: deal with badly scaled vectors (see lapack's larfg)
            *dtau = MAGMA_Z_MAKE((beta - alphar)/beta, -alphai/beta);
            //if (blockIdx.x == gridDim.x-1)
            //*dx0  = MAGMA_Z_MAKE(beta, 0.);
            *dAkk = MAGMA_Z_MAKE(beta, 0.);

            alpha = MAGMA_Z_MAKE( MAGMA_Z_REAL(alpha) - beta, MAGMA_Z_IMAG(alpha));
            scale = MAGMA_Z_DIV( MAGMA_Z_ONE, alpha);
#endif
        }
    }

    // scale x
    __syncthreads();
    if ( xnorm != 0 && j < n-1)
        dx[j] = MAGMA_Z_MUL(dxi, scale);
}


/*
   Generates Householder elementary reflector H = I - tau v v^T to reduce
     H [ dx0 ] = [ beta ]
       [ dx  ]   [ 0    ]
   with beta = ±norm( [dx0, dx] ) = ±dxnorm[0].
   Stores v over dx; first element of v is 1 and is not stored.
   Stores beta over dx0.
   Stores tau.  

   The difference with LAPACK's zlarfg is that the norm of dx, and hence beta,
   are computed outside the routine and passed to it in dxnorm (array on the GPU).
*/
extern "C" void
magma_zlarfg_gpu(int n, magmaDoubleComplex *dx0, magmaDoubleComplex *dx, 
                 magmaDoubleComplex *dtau, double *dxnorm)
{
    dim3 blocks((n+BLOCK_SIZE-1) / BLOCK_SIZE);
    dim3 threads( BLOCK_SIZE );

    /* recomputing the norm */
    //magmablas_dznrm2(n, 1, dx0, n, dxnorm);

    magma_zlarfg_gpu_kernel<<< blocks, threads, 
                               0, magma_stream >>>(n, dx0, dx, dtau, dxnorm );
}

extern "C" void
magma_zlarfg2_gpu(int n, magmaDoubleComplex *dx0, magmaDoubleComplex *dx,
                  magmaDoubleComplex *dtau, double *dxnorm, magmaDoubleComplex *dAkk)
{
    dim3 blocks((n+BLOCK_SIZE-1) / BLOCK_SIZE);
    dim3 threads( BLOCK_SIZE );

    /* recomputing the norm */
    magmablas_dznrm2(n, 1, dx0, n, dxnorm);

    magma_zlarfg2_gpu_kernel<<< blocks, threads,
                               0, magma_stream >>>(n, dx0, dx, dtau, dxnorm, dAkk);
}
