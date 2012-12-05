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
#if (GPUSHMEM < 200)
   #define BLOCK_SIZE 512
#else
   #define BLOCK_SIZE 768
#endif

#define PRECISION_z

__global__ void magma_zgemv_kernel3(int m, const cuDoubleComplex * __restrict__ V, int ldv, 
                                    cuDoubleComplex *c, cuDoubleComplex *dwork,
                                    cuDoubleComplex *tau);
__global__ void magma_ztrmv_kernel(const cuDoubleComplex *T, int ldt, cuDoubleComplex *v);
__global__ void magma_ztrmv_kernel2(const cuDoubleComplex *T, int ldt, 
                                    cuDoubleComplex *v, cuDoubleComplex *y, cuDoubleComplex *tau);

//==============================================================================

__global__
void magma_zlarfgx_gpu_kernel( int n, cuDoubleComplex* dx0, cuDoubleComplex* dx, 
                               cuDoubleComplex *dtau, double *dxnorm,
                               cuDoubleComplex *dA, int it)
{
    const int i = threadIdx.x;
    const int j = i + BLOCK_SIZE * blockIdx.x;
    __shared__ cuDoubleComplex scale;
    __shared__ double xnorm;    
  
    cuDoubleComplex dxi;

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
            if (j==0){
               *dtau = (beta - alpha) / beta;
               //*dx0  = 1.;
               *dA   = beta;  
            }

            scale = 1. / (alpha - beta);
#else
            cuDoubleComplex alpha = *dx0;
            double alphar =  MAGMA_Z_REAL(alpha), alphai = MAGMA_Z_IMAG(alpha);

            // no need to compute the norm as it is passed as input
            double beta  = xnorm; // sqrt( alphar*alphar + alphai*alphai + xnorm*xnorm );
            beta  = -copysign( beta, alphar );

            // todo: deal with badly scaled vectors (see lapack's larfg)
            if (j==0){
               *dtau = MAGMA_Z_MAKE((beta - alphar)/beta, -alphai/beta);
               //*dx0  = MAGMA_Z_MAKE(  1., 0.);
               *dA   = MAGMA_Z_MAKE(beta, 0.);
            }            

            alpha = MAGMA_Z_MAKE( MAGMA_Z_REAL(alpha) - beta, MAGMA_Z_IMAG(alpha));
            scale = MAGMA_Z_DIV( MAGMA_Z_ONE, alpha);
#endif
        }
    }

    // scale x
    __syncthreads();
    if ( xnorm != 0 && j < n-1)
        dx[j] = MAGMA_Z_MUL(dxi, scale);

    if (j<it){
        *( dA-it+j) = *(dx0-it+j);
        *(dx0-it+j) = MAGMA_Z_MAKE(0., 0.);
    } 
}

//==============================================================================

/*
   Generates Householder elementary reflector H = I - tau v v^T to reduce
     H [ dx0 ] = [ beta ]
       [ dx  ]   [ 0    ]
   with beta = ±norm( [dx0, dx] ) = ±dxnorm[0].
   Stores v over dx; first element of v is 1 and is not stored.
   Stores beta over dx0.
   Stores tau.  

   The difference with LAPACK's zlarfg is that the norm of dx, and hance beta,
   are computed outside the routine and passed to it in dxnorm (array on the GPU).
*/
extern "C" void
magma_zlarfgx_gpu(int n, cuDoubleComplex *dx0, cuDoubleComplex *dx, 
                  cuDoubleComplex *dtau, double *dxnorm, 
                  cuDoubleComplex *dA, int it)
{
    dim3 blocks((n+BLOCK_SIZE-1) / BLOCK_SIZE);
    dim3 threads( BLOCK_SIZE );
 
    magma_zlarfgx_gpu_kernel<<< blocks, threads >>>( n, dx0, dx, dtau, dxnorm, dA, it);
}

//==============================================================================

/*
   Generates Householder elementary reflector H = I - tau v v^T to reduce
     H [ dx0 ] = [ beta ]
       [ dx  ]   [ 0    ]
   with beta = ±norm( [dx0, dx] ) = ±dxnorm[0].
   Stores v over dx; first element of v is 1 and is not stored.
   Stores beta over dx0.
   Stores tau.

   The difference with LAPACK's zlarfg is that the norm of dx, and hance beta,
   are computed outside the routine and passed to it in dxnorm (array on the GPU).
*/
extern "C" void
magma_zlarfgtx_gpu(int n, cuDoubleComplex *dx0, cuDoubleComplex *dx,
                   cuDoubleComplex *dtau, double *dxnorm,
                   cuDoubleComplex *dA, int i, 
                   cuDoubleComplex *V, int ldv, cuDoubleComplex *T, int ldt, 
                   cuDoubleComplex *work)
{
   /*  Generate the elementary reflector H(i)  */
   magma_zlarfgx_gpu(n, dx0, dx, dtau, dxnorm, dA, i);

   if (i==0){
      cuDoubleComplex tt = MAGMA_Z_ONE;
      magmablas_zlacpy(MagmaUpperLower, 1, 1, dtau, 1, T+i+i*ldt, 1);
      magma_zsetmatrix(1,1, &tt,1, dx0,1);
   }
   else
   {
      /* Compute the i-th column of T */      
      magma_zgemv_kernel3<<< i, BLOCK_SIZE >>>(n, V, ldv, dx0, work, dtau);
      magma_ztrmv_kernel2<<< i, i          >>>( T, ldt, work, T+i*ldt, dtau);
   }
}

//==============================================================================

