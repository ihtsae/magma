/*
 * Copyright (c) 2011      The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 *
 *
 *     @author Azzam Haidar
 *     @author Stan Tomov
 *
 *     @precisions normal z -> s d c
 *
 */

#include "common_magma.h"
#include "magma_bulge.h"
#include <cblas.h>

//#include "magma_zbulgeinc.h"
// === Define what BLAS to use ============================================
#define PRECISION_z

// nb is assumed to be < BLOCK_SIZE; if not, increase BLOCK_SIZE
// NOTE THAT BLOCK_SIZE should be equal BLOCK_SIZEx*BLOCK_SIZEy
// and BLOCK_SIZEy <= BLOCK_SIZEx
#define BLOCK_SIZE  256
//BLOCK_SIZEx*BLOCK_SIZEy = BLOCK_SIZE
#define BLOCK_SIZEx  64
#define BLOCK_SIZEy   4

//BLKD1_SIZE*BLKD2_SIZE = BLOCK_SIZE
#define BLKD1_SIZE  32 // should always be smaller= than 32 if MAX_NB <128 and less than 16 if MAX_NB 256
#define BLKD2_SIZE  8
#define MAX_NB      128

 

 
// === End defining what BLAS to use ======================================

extern "C" {

    void magma_zlarfxsym_v2(magma_int_t n, cuDoubleComplex *A, magma_int_t lda, cuDoubleComplex *V, cuDoubleComplex *TAU, cuDoubleComplex *work);

}

__device__ void zsum_reduce( int n, int i, cuDoubleComplex* x )
{
    __syncthreads();
    if ( n >  128 ) { if ( i <  128 && i +  128 < n ) { x[i] += x[i+ 128]; }  __syncthreads(); }
    if ( n >   64 ) { if ( i <   64 && i +   64 < n ) { x[i] += x[i+  64]; }  __syncthreads(); }
    if ( n >   32 ) { if ( i <   32 && i +   32 < n ) { x[i] += x[i+  32]; }  __syncthreads(); }
    // probably don't need __syncthreads for < 16 threads
    // because of implicit warp level synchronization.
    if ( n >   16 ) { if ( i <   16 && i +   16 < n ) { x[i] += x[i+  16]; }  __syncthreads(); }
    if ( n >    8 ) { if ( i <    8 && i +    8 < n ) { x[i] += x[i+   8]; }  __syncthreads(); }
    if ( n >    4 ) { if ( i <    4 && i +    4 < n ) { x[i] += x[i+   4]; }  __syncthreads(); }
    if ( n >    2 ) { if ( i <    2 && i +    2 < n ) { x[i] += x[i+   2]; }  __syncthreads(); }
    if ( n >    1 ) { if ( i <    1 && i +    1 < n ) { x[i] += x[i+   1]; }  __syncthreads(); }
}


 __device__ void sum_reduce(int n, int i, double* x )
{
    __syncthreads();
    if ( n >  128 ) { if ( i <  128 && i +  128 < n ) { x[i] += x[i+ 128]; }  __syncthreads(); }
    if ( n >   64 ) { if ( i <   64 && i +   64 < n ) { x[i] += x[i+  64]; }  __syncthreads(); }
    if ( n >   32 ) { if ( i <   32 && i +   32 < n ) { x[i] += x[i+  32]; }  __syncthreads(); }
    // probably don't need __syncthreads for < 16 threads
    // because of implicit warp level synchronization.
    if ( n >   16 ) { if ( i <   16 && i +   16 < n ) { x[i] += x[i+  16]; }  __syncthreads(); }
    if ( n >    8 ) { if ( i <    8 && i +    8 < n ) { x[i] += x[i+   8]; }  __syncthreads(); }
    if ( n >    4 ) { if ( i <    4 && i +    4 < n ) { x[i] += x[i+   4]; }  __syncthreads(); }
    if ( n >    2 ) { if ( i <    2 && i +    2 < n ) { x[i] += x[i+   2]; }  __syncthreads(); }
    if ( n >    1 ) { if ( i <    1 && i +    1 < n ) { x[i] += x[i+   1]; }  __syncthreads(); }
}

__device__ void sum_rowreduce_2d( int n, int thxid, int thyid, cuDoubleComplex x[][BLOCK_SIZEy+1] )
{
    __syncthreads();
    if ( n > 1024 ) { if ( thxid < 1024 && thxid + 1024 < n ) { x[thxid][thyid] += x[thxid+1024][thyid]; }  __syncthreads(); }
    if ( n >  512 ) { if ( thxid <  512 && thxid +  512 < n ) { x[thxid][thyid] += x[thxid+ 512][thyid]; }  __syncthreads(); }
    if ( n >  256 ) { if ( thxid <  256 && thxid +  256 < n ) { x[thxid][thyid] += x[thxid+ 256][thyid]; }  __syncthreads(); }
    if ( n >  128 ) { if ( thxid <  128 && thxid +  128 < n ) { x[thxid][thyid] += x[thxid+ 128][thyid]; }  __syncthreads(); }
    if ( n >   64 ) { if ( thxid <   64 && thxid +   64 < n ) { x[thxid][thyid] += x[thxid+  64][thyid]; }  __syncthreads(); }
    if ( n >   32 ) { if ( thxid <   32 && thxid +   32 < n ) { x[thxid][thyid] += x[thxid+  32][thyid]; }  __syncthreads(); }
    // probably don't need __syncthreads for < 16 threads
    // because of implicit warp level synchronization.
    if ( n >   16 ) { if ( thxid <   16 && thxid +   16 < n ) { x[thxid][thyid] += x[thxid+  16][thyid]; }  __syncthreads(); }
    if ( n >    8 ) { if ( thxid <    8 && thxid +    8 < n ) { x[thxid][thyid] += x[thxid+   8][thyid]; }  __syncthreads(); }
    if ( n >    4 ) { if ( thxid <    4 && thxid +    4 < n ) { x[thxid][thyid] += x[thxid+   4][thyid]; }  __syncthreads(); }
    if ( n >    2 ) { if ( thxid <    2 && thxid +    2 < n ) { x[thxid][thyid] += x[thxid+   2][thyid]; }  __syncthreads(); }
    if ( n >    1 ) { if ( thxid <    1 && thxid +    1 < n ) { x[thxid][thyid] += x[thxid+   1][thyid]; }  __syncthreads(); }
}


__device__ void sum_rowreduce_2dn( int nrow, int thxid, int thyid, cuDoubleComplex x[BLKD2_SIZE][BLKD1_SIZE] )
{
    __syncthreads();
    if ( nrow > 1024 ) { if ( thxid < 1024 && thxid + 1024 < nrow ) { x[thxid][thyid] += x[thxid+1024][thyid]; }  __syncthreads(); }
    if ( nrow >  512 ) { if ( thxid <  512 && thxid +  512 < nrow ) { x[thxid][thyid] += x[thxid+ 512][thyid]; }  __syncthreads(); }
    if ( nrow >  256 ) { if ( thxid <  256 && thxid +  256 < nrow ) { x[thxid][thyid] += x[thxid+ 256][thyid]; }  __syncthreads(); }
    if ( nrow >  128 ) { if ( thxid <  128 && thxid +  128 < nrow ) { x[thxid][thyid] += x[thxid+ 128][thyid]; }  __syncthreads(); }
    if ( nrow >   64 ) { if ( thxid <   64 && thxid +   64 < nrow ) { x[thxid][thyid] += x[thxid+  64][thyid]; }  __syncthreads(); }
    if ( nrow >   32 ) { if ( thxid <   32 && thxid +   32 < nrow ) { x[thxid][thyid] += x[thxid+  32][thyid]; }  __syncthreads(); }
    // probably don't need __syncthreads for < 16 threads
    // because of implicit warp level synchronization.
    if ( nrow >   16 ) { if ( thxid <   16 && thxid +   16 < nrow ) { x[thxid][thyid] += x[thxid+  16][thyid]; }  __syncthreads(); }
    if ( nrow >    8 ) { if ( thxid <    8 && thxid +    8 < nrow ) { x[thxid][thyid] += x[thxid+   8][thyid]; }  __syncthreads(); }
    if ( nrow >    4 ) { if ( thxid <    4 && thxid +    4 < nrow ) { x[thxid][thyid] += x[thxid+   4][thyid]; }  __syncthreads(); }
    if ( nrow >    2 ) { if ( thxid <    2 && thxid +    2 < nrow ) { x[thxid][thyid] += x[thxid+   2][thyid]; }  __syncthreads(); }
    if ( nrow >    1 ) { if ( thxid <    1 && thxid +    1 < nrow ) { x[thxid][thyid] += x[thxid+   1][thyid]; }  __syncthreads(); }
}

__device__ void sum_colreduce_2dn(int ncol, int thxid, int thyid, cuDoubleComplex x[BLKD1_SIZE][BLKD2_SIZE+1] )
{
    __syncthreads();
    if ( ncol > 1024 ) { if ( thyid < 1024 && thyid + 1024 < ncol ) { x[thxid][thyid] += x[thxid][thyid+1024]; }  __syncthreads(); }
    if ( ncol >  512 ) { if ( thyid <  512 && thyid +  512 < ncol ) { x[thxid][thyid] += x[thxid][thyid+ 512]; }  __syncthreads(); }
    if ( ncol >  256 ) { if ( thyid <  256 && thyid +  256 < ncol ) { x[thxid][thyid] += x[thxid][thyid+ 256]; }  __syncthreads(); }
    if ( ncol >  128 ) { if ( thyid <  128 && thyid +  128 < ncol ) { x[thxid][thyid] += x[thxid][thyid+ 128]; }  __syncthreads(); }
    if ( ncol >   64 ) { if ( thyid <   64 && thyid +   64 < ncol ) { x[thxid][thyid] += x[thxid][thyid+  64]; }  __syncthreads(); }
    if ( ncol >   32 ) { if ( thyid <   32 && thyid +   32 < ncol ) { x[thxid][thyid] += x[thxid][thyid+  32]; }  __syncthreads(); }
    // probably don't need __syncthreads for < 16 threads
    // because of implicit warp level synchronization.
    if ( ncol >   16 ) { if ( thyid <   16 && thyid +   16 < ncol ) { x[thxid][thyid] += x[thxid][thyid+  16]; }  __syncthreads(); }
    if ( ncol >    8 ) { if ( thyid <    8 && thyid +    8 < ncol ) { x[thxid][thyid] += x[thxid][thyid+   8]; }  __syncthreads(); }
    if ( ncol >    4 ) { if ( thyid <    4 && thyid +    4 < ncol ) { x[thxid][thyid] += x[thxid][thyid+   4]; }  __syncthreads(); }
    if ( ncol >    2 ) { if ( thyid <    2 && thyid +    2 < ncol ) { x[thxid][thyid] += x[thxid][thyid+   2]; }  __syncthreads(); }
    if ( ncol >    1 ) { if ( thyid <    1 && thyid +    1 < ncol ) { x[thxid][thyid] += x[thxid][thyid+   1]; }  __syncthreads(); }
}


///////////////////////////////////////////////////////////
//// add -1 because of C
#define dA(i,j)   &(dA[((i)-(j)) + ldda*((j)-1)])
#define dAC(i,j)   &(dA[(i) + ldda*(j)])

#define dV(i)     &(dV[(i)])
#define dTAU(i)   &(dTAU[(i)])

__device__ void zlarfxsym_v2(magma_int_t n, 
                             cuDoubleComplex *dA, magma_int_t ldda, 
                             cuDoubleComplex *dV, cuDoubleComplex *dTAU) 
{
/*
    WORK (workspace) double complex array, dimension N
*/

    magma_int_t j,nint,gbrow,gbcol,blkjcol;
    cuDoubleComplex dtmp     = MAGMA_Z_ZERO;
    cuDoubleComplex c_half   =  MAGMA_Z_HALF;
    const int myrow = threadIdx.x % BLOCK_SIZEx, mycol= threadIdx.x / BLOCK_SIZEx,  thid = threadIdx.x;

    __shared__ cuDoubleComplex loctau;
   // __shared__ cuDoubleComplex locw[ BLOCK_SIZE ];
    __shared__ cuDoubleComplex locv[ BLOCK_SIZE ];
    __shared__ cuDoubleComplex loca[ BLOCK_SIZEx ][ BLOCK_SIZEx+1 ];
    __shared__ cuDoubleComplex sum[ BLOCK_SIZE ][ BLOCK_SIZEy+1];

    __syncthreads();
    locv[thid] = thid < n ? dV[thid] : MAGMA_Z_ZERO;
    if(thid==0) loctau     = dTAU[0];
    __syncthreads();
   
    // initialize all the column of sum (BLOCK_SIZEy col) to zero
    /* 
    for( j = myrow; j < BLOCK_SIZE; j+= BLOCK_SIZEx)
    {
        sum[j][mycol] = MAGMA_Z_ZERO;
    }
    */
    for( j = 0; j < BLOCK_SIZEy; j++){
         sum[thid][j] = MAGMA_Z_ZERO;
    }  
    //if(thid==0)printf("hello"); return;
    __syncthreads();



    /* 
        X = tau A V 
        blasf77_zhemv("L", &n, TAU, A, &lda, V, &ione, &c_zero, work, &ione);
    */

    j = n%BLOCK_SIZEx;
    nint = j == 0 ? n : n - j; 
    //printf("me %d nint %d\n",thid,myrow, nint);
    
    // go over the blocki (vertical down) excluding the last block in case of padding required
    for(gbrow = myrow; gbrow<nint; gbrow+=BLOCK_SIZEx){
        //if(thid==0)printf("%d  ===============  HELLO FROM THE MAIN LOOP  ================= \n", thid); __syncthreads();

        // go over the blockj (horizontal left to right)
        // excluding diagonal block which is treated after it
        blkjcol = (gbrow/BLOCK_SIZEx)*BLOCK_SIZEx;
        for( gbcol = 0; gbcol<blkjcol; gbcol+=BLOCK_SIZEx){
            //if(thid==0)printf("%d ===============> MAIN LOOP  offDIAG BLOCK gbrow %d   gbcol %d \n", thid, gbrow, gbcol); __syncthreads();

            // for non diag block, copy the matrix to shared,
            // and directly do the first GEMV (threads horizontal reading)  
            // then another loop will do the second GEMV 
            // with the transpose (vertical reading)  
            for( j = mycol; j < BLOCK_SIZEx; j+= BLOCK_SIZEy)
            {
                loca[myrow][j] = *(dAC(gbrow, (gbcol+j))) ;  
                sum[gbrow][mycol] += loca[myrow][j] * locv[gbcol+j];
            }
            __syncthreads();
            for( j = mycol; j < BLOCK_SIZEx; j+= BLOCK_SIZEy)
            {
                sum[gbcol+myrow][mycol] += MAGMA_Z_CNJG(loca[j][myrow]) * locv[blkjcol+j];
            }
            __syncthreads();
        }
        // the diagonal block
        gbcol = blkjcol;
        //if(thid==0)printf("%d  ===============>  DIAG BLOCK  myrow %d mycol %d gbrow %d   gbcol %d \n", thid,myrow, mycol, gbrow, gbcol); __syncthreads();
        for( j = mycol; j <= myrow; j+= BLOCK_SIZEy)
        {
            loca[myrow][j] = *(dAC(gbrow, (gbcol+j))) ; 
            loca[j][myrow] =  MAGMA_Z_CNJG( loca[myrow][j] );
        }
        __syncthreads();

        for( j = mycol; j < BLOCK_SIZEx; j+= BLOCK_SIZEy)
        {
            sum[gbrow][mycol] += loca[myrow][j] * locv[gbcol+j];
        }
        __syncthreads();
    }
    // In case where a padding should exist and is not, so let do the last block in case of its size < BLOCK_SIZEx independently


    if(nint<n){

        gbrow = nint+myrow;
        blkjcol = (gbrow/BLOCK_SIZEx)*BLOCK_SIZEx;    
        for( gbcol = 0; gbcol<blkjcol; gbcol+=BLOCK_SIZEx){
            //printf("%d  LAST LOOP  gbrow %d   gbcol %d \n", thid, gbrow, gbcol);
            if(gbrow<n){
                for( j = mycol; j < BLOCK_SIZEx; j+= BLOCK_SIZEy)
                {
                    loca[myrow][j] = *(dAC(gbrow, (gbcol+j))) ;  
                    sum[gbrow][mycol] += loca[myrow][j] * locv[gbcol+j];
                }
            }
            __syncthreads();

            for( j = mycol; j < n-nint; j+= BLOCK_SIZEy)
            {
                sum[gbcol+myrow][mycol] += MAGMA_Z_CNJG(loca[j][myrow]) * locv[blkjcol+j];
            }
            __syncthreads();
        }
        // the diagonal block
        gbcol = blkjcol;
        //printf("%d  LAST DIAG BLOCK  gbrow %d   gbcol %d \n", thid, gbrow, gbcol);
        if(gbrow<n){
            for( j = mycol; j <= myrow; j+= BLOCK_SIZEy)
            {
                loca[myrow][j] = *(dAC(gbrow, (gbcol+j))) ; 
                loca[j][myrow] =  MAGMA_Z_CNJG( loca[myrow][j] );
            }
        }
        __syncthreads();
        if(gbrow<n){
            for( j = mycol; j <  n-nint; j+= BLOCK_SIZEy)
            {
                sum[gbrow][mycol] += loca[myrow][j] * locv[gbcol+j];
            }
        }
        __syncthreads();

    }
    
    // The result of the GEMV is now in sum[1:n][BLOCK_SIZEy]
    // and need to be summed over the BLOCK_SIZEy.
    // each thread go over the BLOCK_SIZEy and summ it to its sum[thid][0]
    for( j = 1; j < BLOCK_SIZEy; j++){
        sum[thid][0] += sum[thid][j];
    }
    sum[thid][1] = loctau * sum[thid][0];


    sum[thid][0]  = MAGMA_Z_ZERO;    
    if(thid<n){
        /* compute dtmp= X'*V */
        sum[thid][0] = MAGMA_Z_CNJG( sum[thid][1] ) * locv[thid];
    }
    sum_rowreduce_2d(n, thid, 0, sum);

    if(thid<n){
        /* compute 1/2 X'*V*t = 1/2*dtmp*tau  */
        dtmp = sum[0][0] * c_half * loctau;
        /*
           compute W=X-1/2VX'Vt = X - dtmp*V 
           blasf77_zaxpy(&n, &dtmp, V, &ione, work, &ione); 
        */
        sum[thid][1] -= dtmp * locv[thid]; 
    }
    /* 
       performs the symmetric rank 2 operation A := alpha*x*y' + alpha*y*x' + A 
       blasf77_zher2("L", &n, &c_neg_one, work, &ione, V, &ione, A, &lda);
    */
    __syncthreads();    
    if(thid<n){
        if( n <= BLOCK_SIZEx){ // meaningthat the matrix is fully loaded into shared so use it
            for(j=0; j<=thid; j++)
               *dAC(thid, j) = loca[thid][j] - sum[thid][1]*MAGMA_Z_CNJG( locv[j] ) - locv[thid]*MAGMA_Z_CNJG( sum[j][1] ); 
            // *dAC(thid, thid) = MAGMA_Z_MAKE( MAGMA_Z_REAL(*dAC(thid, thid)), 0.);
        }else{        
            for(j=0; j<=thid; j++)
               *dAC(thid, j) -= sum[thid][1]*MAGMA_Z_CNJG( locv[j] ) + locv[thid]*MAGMA_Z_CNJG( sum[j][1] ); 
            // *dAC(thid, thid) = MAGMA_Z_MAKE( MAGMA_Z_REAL(*dAC(thid, thid)), 0.);
        }
    }
    



    // synch the routine
    __syncthreads();

}

///////////////////////////////////////////////////////////
//                  TYPE 1-BAND Householder
///////////////////////////////////////////////////////////
__device__ void zlarfg(int n, cuDoubleComplex *dA, cuDoubleComplex *dx,
                       cuDoubleComplex *dtau)
{
    const int i = threadIdx.x;
    __shared__ cuDoubleComplex scale;
    __shared__ double sum[ BLOCK_SIZE ], beta;
    cuDoubleComplex alpha;

#if (defined(PRECISION_s) || defined(PRECISION_d))
#else
    double alphar;
    __shared__ double alphai;
#endif



    __syncthreads();
#if (defined(PRECISION_s) || defined(PRECISION_d))
    if( n <= 1 ) {
#else
    if( n <= 0 ) {
#endif
        *dtau = MAGMA_Z_ZERO;
        return;
    }

    //sum[i] = MAGMA_D_ZERO

   
/*
#if (defined(PRECISION_c) || defined(PRECISION_z))
    if (( n == 1 ) &&( i==0 ) ){
       sum[0] = MAGMA_D_ZERO
    }
#endif
*/

    /* Compute the norm of dx
      XNORM = DZNRM2( N-1, X, INCX )
    */
    if (i<n-1){
#if (defined(PRECISION_s) || defined(PRECISION_d))
         {
         double re = dx[i];
         sum[i] = re*re;
         }
#else
         {
         double re = MAGMA_Z_REAL(dx[i]), im = MAGMA_Z_IMAG(dx[i]);
         sum[i] = re*re + im*im;
         }
#endif
    }else{
        sum[i] = MAGMA_D_ZERO;
    }
    // we need a sync here but because sum_reduce has a sync implicitly at the top so we comment it
    //__syncthreads();
    sum_reduce( n-1, i, sum );


    if ( i == 0 ) {
    alpha = *dA;
#if (defined(PRECISION_s) || defined(PRECISION_d))
    beta = sqrt(sum[0]);
#else
    alphar = MAGMA_Z_REAL(alpha);
    alphai = MAGMA_Z_IMAG(alpha);
    if ( n == 1 )
        beta = MAGMA_D_ZERO;
    else
        beta = sqrt(sum[0]);
#endif 
    }
    __syncthreads();



#if (defined(PRECISION_s) || defined(PRECISION_d))
    if( beta == 0) {
#else
    if( beta == 0 && alphai == 0) {
#endif
        *dtau = MAGMA_Z_ZERO;
        return;
    }

    if ( i == 0 ) {
#if (defined(PRECISION_s) || defined(PRECISION_d))
            beta  = beta*beta + alpha*alpha;
            beta  = sqrt(beta);
            beta  = -copysign( beta, alpha );

            // todo: deal with badly scaled vectors (see lapack's larfg)
            *dtau = (beta - alpha) / beta;
            *dA = beta;

            scale = 1. / (alpha - beta);
#else
            beta  = beta*beta + alphar*alphar + alphai*alphai;
            beta  = sqrt(beta);
            beta  = -copysign( beta, alphar );

            // todo: deal with badly scaled vectors (see lapack's larfg)
            *dtau = MAGMA_Z_MAKE((beta - alphar)/beta, -alphai/beta);

            *dA = MAGMA_Z_MAKE(beta, 0.);

            alpha = MAGMA_Z_MAKE( MAGMA_Z_REAL(alpha) - beta, MAGMA_Z_IMAG(alpha));
            scale = MAGMA_Z_DIV( MAGMA_Z_ONE, alpha);
#endif
    }

    // scale x
    __syncthreads();
    if ( i < n-1)
        dx[i] = MAGMA_Z_MUL(dx[i], scale);



    // synch the routine
    __syncthreads();
    
}


__global__
void magma_ztrdtype1cbHLsym_withQ_v2_gpu_kernel(cuDoubleComplex *dA, int ldda,
                                                cuDoubleComplex *dV, cuDoubleComplex *dTAU,
                                                int st, int len)
{
       const int thid = threadIdx.x;

          /*
             V(0)  = c_one;
             cblas_zcopy(len-1, A(st+1, st-1), ione, V(1), ione);
             memset(A(st+1, st-1), 0, (len-1)*sizeof(cuDoubleComplex));
          */
          if (thid==0){
             dV[0] = MAGMA_Z_ONE;
          } else if(thid < len){
             dV[thid] = *dA(st+thid, st-1);
             *dA(st+thid, st-1) = MAGMA_Z_ZERO;
          }
       
          /*
             Eliminate the col  at st-1
             lapackf77_zlarfg( &len, A(st, st-1), V(1), &ione, TAU );
          */
          zlarfg(len, dA(st,st-1), dV(1), dTAU);

          /*
             apply left and right on A(st:ed,st:ed)
             magma_zlarfxsym_v2(len, A(st,st), lda-1, V, TAU, work);
          */
          zlarfxsym_v2(len, dA(st,st), ldda-1, dV, dTAU);
}

extern "C" void
magma_ztrdtype1cbHLsym_withQ_v2_gpu(magma_int_t n, magma_int_t nb, 
                                    cuDoubleComplex *dA, magma_int_t ldda, 
                                    cuDoubleComplex *dV, magma_int_t lddv, 
                                    cuDoubleComplex *dTAU,
                                    magma_int_t st, magma_int_t ed, magma_int_t sweep, 
                                    magma_int_t Vblksiz, cuDoubleComplex *dwork) 
{
/*
    WORK (workspace) double complex array, dimension N
*/
    magma_int_t vpos, taupos, len;
    //magma_int_t lddx = ldda-1;

    if (nb > BLOCK_SIZE)
       printf("magma_ztrdtype1cbHLsym_withQ_v2_gpu: BLOCK_SIZE should be > %d\n", nb);
 
    magma_bulge_findVTAUpos(n, nb, Vblksiz, sweep-1, st-1, lddv, &vpos, &taupos);
    //printf("voici vpos %d taupos %d  tpos %d  blkid %d \n", vpos, taupos, tpos, blkid);

    len = ed-st+1;

    /* === Compute the following using one multiprocessor with BLOCK_SIZE threads ===
       *V(vpos)  = c_one;
       cblas_zcopy(len-1, A(st+1, st-1), ione, V(vpos+1), ione);
       memset(A(st+1, st-1), 0, (len-1)*sizeof(cuDoubleComplex));

       // Eliminate the col  at st-1 
       lapackf77_zlarfg( &len, A(st, st-1), V(vpos+1), &ione, TAU(taupos) );

       // apply left and right on A(st:ed,st:ed)
       magma_zlarfxsym_v2(len, A(st,st), lda-1, V(vpos), TAU(taupos), work);
    */ 
    magma_ztrdtype1cbHLsym_withQ_v2_gpu_kernel<<<1, BLOCK_SIZE>>>(dA, ldda, dV+vpos,
                                                                  dTAU+taupos,
                                                                  st, len);
}
#undef dA
#undef dAC
#undef dV
#undef dTAU


























__device__ void zlarfr000(int m, int n, cuDoubleComplex *v, cuDoubleComplex dtau, 
                       cuDoubleComplex *c, int ldc)
{
   __syncthreads();
   const int thid = threadIdx.x;
   __shared__ cuDoubleComplex loctau;
   if(thid==0) loctau     = dtau;
   __syncthreads();

   if ( !MAGMA_Z_EQUAL(loctau, MAGMA_Z_ZERO) ) {
         __shared__ cuDoubleComplex locv[ BLOCK_SIZE ];
        //__shared__ cuDoubleComplex loca[ BLOCK_SIZEx ][ BLOCK_SIZEx+1 ];
        //__shared__ cuDoubleComplex sum[ BLOCK_SIZE ][ BLOCK_SIZEy+1];

        locv[thid] = thid < n ? v[thid] : MAGMA_Z_ZERO;
        __syncthreads();


        if (thid<m) {
           cuDoubleComplex *dc = c + thid;
           cuDoubleComplex lsum = MAGMA_Z_ZERO;

           /*  w := C  * v  */
           for( int j = 0; j < n; j ++ )
              lsum += dc[j*ldc] * locv[j] ;


           /*  C := C - tau * w * v'  */
           //__syncthreads();
           lsum = - loctau * lsum;
           for( int j = 0; j<n ; j ++ ) 
               dc[j*ldc] += lsum * MAGMA_Z_CNJG( locv[j] );
        }
        // synch the routine
        __syncthreads();
  }
}

///////////////////////////////////////////////////////////
//                  TYPE 1-LPK Householder
///////////////////////////////////////////////////////////
//// add -1 because of C
#define dA(i,j)    &(dA[((i)-(j)) + ldda*((j)-1)])
#define dAC(i,j)   &(dA[(i) + ldda*(j)])

#define   dV(i)     &(dV[(i)])
#define dTAU(i)   &(dTAU[(i)])


/* Applies a complex elementary reflector H to a complex m by n
   matrix C, from the right. H is represented in the form

        H = I - tau * v * v'

   where tau is a complex scalar and v is a complex vector.
   If tau = 0, then H is taken to be the unit matrix              */
//====================================================================================================
//    RIGHT UPDATE 2D BLOCKED
//====================================================================================================
__device__ void zlarfr(int m, int n, cuDoubleComplex *v, cuDoubleComplex dtau, 
                       cuDoubleComplex *c, int ldc)
{

   const int thid = threadIdx.x;
   __shared__ cuDoubleComplex loctau;
   __syncthreads();


   if(thid==0) loctau     = dtau;
   __syncthreads();

   if ( !MAGMA_Z_EQUAL(loctau, MAGMA_Z_ZERO) ) {
      cuDoubleComplex lsum = MAGMA_Z_ZERO;
      magma_int_t j, gbrow, mpad;
      const int myrow = threadIdx.x % BLKD1_SIZE, mycol= threadIdx.x / BLKD1_SIZE;
      __shared__ cuDoubleComplex locv[ MAX_NB ];
      __shared__ cuDoubleComplex loca[ BLKD1_SIZE ][ MAX_NB+1 ];
      __shared__ cuDoubleComplex sum[ BLKD1_SIZE ][ BLKD2_SIZE+1];

      if(thid<n)
          locv[thid] = v[thid];
      __syncthreads();
       
      mpad = ((m+BLKD1_SIZE-1)/BLKD1_SIZE)*BLKD1_SIZE;
      // go over the blocki (vertical down)
      for(gbrow = myrow; gbrow<mpad; gbrow+=BLKD1_SIZE){
          sum[myrow][mycol] = MAGMA_Z_ZERO;
          // read a block of size BLKD1_SIZE x BLKD2_SIZE and do the GEMV
          // w := C  * v  
          if(gbrow<m){
              for( j = mycol; j < n; j+= BLKD2_SIZE)
                  loca[myrow][j]     = c[gbrow+j*ldc];
              for( j = mycol; j < n; j+= BLKD2_SIZE)
                  sum[myrow][mycol] += loca[myrow][j] * locv[j] ;
          }
          sum_colreduce_2dn(BLKD2_SIZE, myrow, mycol, sum);

          //  C := C - tau * w * v' 
          if(gbrow<m){
              /*
              if(mycol == 0){
                  sum[myrow][0] = -loctau *sum[myrow][0];
              }
              __syncthreads();
              for( j = mycol; j < n; j+= BLKD2_SIZE)
                  c[gbrow+j*ldc]  = loca[myrow][j]  + sum[myrow][0] * MAGMA_Z_CNJG( locv[j] );
              */
                  
              lsum = -loctau * sum[myrow][0];
              /*
              for( j = mycol; j < n; j+= BLKD2_SIZE)
                  loca[myrow][j] += lsum * MAGMA_Z_CNJG( locv[j] );
              for( j = mycol; j < n; j+= BLKD2_SIZE)
                  c[gbrow+j*ldc]  = loca[myrow][j];               */
              for( j = mycol; j < n; j+= BLKD2_SIZE)
                  c[gbrow+j*ldc]  = loca[myrow][j] + lsum * MAGMA_Z_CNJG( locv[j] );
                  
          }
          // sync between the blocki but ithink i don't need it here because every thread work on its same loca
          __syncthreads();
      }
  }
  // synch the routine
  __syncthreads();
}
//====================================================================================================

//====================================================================================================
//    LEFT UPDATE 2D BLOCKED
//====================================================================================================
__device__ void zlarfl(int m, int n, cuDoubleComplex *v, cuDoubleComplex dtau, 
                       cuDoubleComplex *c, int ldc)
{

   const int thid = threadIdx.x;
   __shared__ cuDoubleComplex loctau;
   __syncthreads();


   if(thid==0) loctau     = dtau;
   __syncthreads();

   if ( !MAGMA_Z_EQUAL(loctau, MAGMA_Z_ZERO) ) {
      cuDoubleComplex lsum = MAGMA_Z_ZERO;
      magma_int_t j, gbcol, npad;
      magma_int_t idlastblk,blkid,blknbcol,blksize,irow,icol,gbrow,blkjcol;
      const int mycol = threadIdx.x % BLKD1_SIZE, myrow= threadIdx.x / BLKD1_SIZE;
      __shared__ cuDoubleComplex locv[ MAX_NB ];
      __shared__ cuDoubleComplex loca[ MAX_NB ] [ BLKD1_SIZE +1];
      __shared__ cuDoubleComplex sum[ BLKD2_SIZE] [ BLKD1_SIZE ];

      if(thid<m)
          locv[thid] = v[thid];
      __syncthreads();

       
      npad = ((n+BLKD1_SIZE-1)/BLKD1_SIZE)*BLKD1_SIZE;
      idlastblk = (npad/BLKD1_SIZE)-1;
      // go over the blocki (vertical down)
      for(gbcol = mycol; gbcol<npad; gbcol+=BLKD1_SIZE){
          sum[myrow][mycol] = MAGMA_Z_ZERO;
/*
          blkid    = gbcol/BLKD1_SIZE;
          blknbcol = blkid == idlastblk?  n-(idlastblk*BLKD1_SIZE) : BLKD1_SIZE;
          blksize  = blknbcol * m; 

          for( j = thid; j < blksize; j+= BLOCK_SIZE) { 
              irow = j%m;
              icol = j/m;
              loca[irow][icol]     = c[irow+(blkid*BLKD1_SIZE+icol)*ldc];
          }
          __syncthreads();
*/
          blkid    = gbcol/BLKD1_SIZE;
          blknbcol = blkid == idlastblk?  n-(idlastblk*BLKD1_SIZE) : BLKD1_SIZE;
          blkjcol  = blkid*BLKD1_SIZE;
          for(gbrow = mycol; gbrow<m; gbrow+=BLKD1_SIZE){
              for( j = myrow; j < blknbcol; j+= BLKD2_SIZE)
                  loca[gbrow][j]     = c[gbrow+(blkjcol+j)*ldc];
          }
          __syncthreads();

          // read a block of size BLKD1_SIZE x BLKD2_SIZE and do the GEMV
          // w := C  * v  
          if(gbcol<n){
              for( j = myrow; j < m; j+= BLKD2_SIZE)
              {
                  //loca[j][mycol]     = c[j+gbcol*ldc];
                  sum[myrow][mycol] += loca[j][mycol] * MAGMA_Z_CNJG( locv[j] );
              }
          }
          sum_rowreduce_2dn(BLKD2_SIZE, myrow, mycol, sum);

          //  C := C - tau * w * v' 
          if(gbcol<n){
              /*
              if(myrow == 0){
                  sum[0][mycol] = -loctau *sum[0][mycol];
              }
              __syncthreads();
              for( j = myrow; j < m; j+= BLKD2_SIZE)
                  c[j+gbcol*ldc]  = loca[j][mycol]  + sum[0][mycol] * locv[j];
              */
                  
              lsum = -loctau * sum[0][mycol];
              for( j = myrow; j < m; j+= BLKD2_SIZE)
                  loca[j][mycol]  += lsum * locv[j];                  
          }
/*
          for( j = thid; j < blksize; j+= BLOCK_SIZE) { 
              irow = j%m;
              icol = j/m;
              c[irow+(blkid*BLKD1_SIZE+icol)*ldc] = loca[irow][icol];
          }
          __syncthreads();
*/
          for(gbrow = mycol; gbrow<m; gbrow+=BLKD1_SIZE){
              for( j = myrow; j < blknbcol; j+= BLKD2_SIZE)
                  c[gbrow+(blkjcol+j)*ldc] = loca[gbrow][j];
          }
          // sync between the blocki but ithink i don't need it here because every thread work on its same loca
          __syncthreads();
      }
  }
  // synch the routine
  __syncthreads();
}
//====================================================================================================






__device__ void zlarfl000(int m, int n, cuDoubleComplex *v, cuDoubleComplex dtau, 
                       cuDoubleComplex *c, int ldc)
{
   __syncthreads();
   const int thid = threadIdx.x;
   __shared__ cuDoubleComplex loctau;
   if(thid==0) loctau     = dtau;
   __syncthreads();

   if ( !MAGMA_Z_EQUAL(loctau, MAGMA_Z_ZERO) ) {
        __shared__ cuDoubleComplex locv[ BLOCK_SIZE ];
        //__shared__ cuDoubleComplex loca[ BLOCK_SIZEx ][ BLOCK_SIZEx+1 ];
        //__shared__ cuDoubleComplex sum[ BLOCK_SIZE ][ BLOCK_SIZEy+1];

        locv[thid] = thid < m ? v[thid] : MAGMA_Z_ZERO;
        __syncthreads();


        if (thid<n) {
           cuDoubleComplex *dc = c + thid*ldc;
           cuDoubleComplex lsum = MAGMA_Z_ZERO;
           /*  w := v'  * C  */
           for( int j = 0; j < m; j ++ )
              lsum +=  dc[j]* MAGMA_Z_CNJG(locv[j]);


           /*  C := C - tau * v * w */
           //__syncthreads();
           lsum = - loctau * lsum;
           for( int j = 0; j<m ; j ++ ) 
               dc[j] += lsum *  locv[j];
        }
        // synch the routine
        __syncthreads();
  }
}













__global__ void
magma_zlarfr_gpu_kernel(int lem, int len, cuDoubleComplex *dV, cuDoubleComplex *dTAU,
                        cuDoubleComplex *dA, int ldda)
{
    zlarfr(lem, len, dV, dTAU[0], dA, ldda);
}

__global__ void
magma_ztrdtype2cbHLsym_withQ_v2_gpu_kernel(int lem, int len,
                                           cuDoubleComplex *dA, int ldda,
                                           cuDoubleComplex *dV, cuDoubleComplex *dTAU,
                                           int st, int ed)
{
     const int thid = threadIdx.x;


     if (lem > 0) {
        /* === Compute the following using one multiprocessor with BLOCK_SIZE threads ===

             // remove the first column of the created bulge
             *V(vpos)  = c_one;
             cblas_zcopy(lem-1, A(ed+2, st), ione, V(1), ione);
             memset(A(ed+2, st),0,(lem-1)*sizeof(cuDoubleComplex));
        */

        if (thid==0){
             dV[0] = MAGMA_Z_ONE;
        } else if (thid<lem) {
             dV[thid] = *dA(ed+1+thid, st);
             *dA(ed+1+thid, st) = MAGMA_Z_ZERO;
        }

        /* === Compute the following using one multiprocessor with BLOCK_SIZE threads ===

             // Eliminate the col at st
             lapackf77_zlarfg( &lem, A(ed+1, st), V(1), &ione, TAU );

        */
   
        zlarfg(lem, dA(ed+1, st), dV(1), dTAU);
     }
        /* === Compute the following using one multiprocessor with BLOCK_SIZE threads ===

             // apply left on A(J1:J2,st+1:ed)
             len = len-1; // because we start at col st+1 instead of st. col st is the col that has been removed
             conjtmp = MAGMA_Z_CNJG(*TAU(taupos));
             lapackf77_zlarfx("L", &lem, &len, V(vpos),  &conjtmp, A(ed+1, st+1), &ldx, work);
        */

       // note that all htreads need to call this function
        zlarfl(lem, len-1, dV, MAGMA_Z_CNJG( dTAU[0] ), dA(ed+1, st+1), ldda-1);
}

extern "C" void
magma_ztrdtype2cbHLsym_withQ_v2_gpu(magma_int_t n, magma_int_t nb, 
                                    cuDoubleComplex *dA, magma_int_t ldda, 
                                    cuDoubleComplex *dV, magma_int_t lddv, 
                                    cuDoubleComplex *dTAU,
                                    magma_int_t st, magma_int_t ed, magma_int_t sweep, 
                                    magma_int_t Vblksiz) 
{
    magma_int_t vpos, taupos;

    magma_int_t lddx = ldda-1;
    magma_int_t len = ed - st + 1;
    magma_int_t lem = min(ed+nb, n) - ed;

    if (nb > BLOCK_SIZE)
       printf("magma_ztrdtype2cbHLsym_withQ_v2_gpu: BLOCK_SIZE should be > %d\n", nb);

    if(lem>0){
        magma_bulge_findVTAUpos(n, nb, Vblksiz, sweep-1, st-1, lddv, &vpos, &taupos);

        /* 
           Apply remaining right coming from the top block using multiprocessor 
           with BLOCK_SIZE threads

           lapackf77_zlarfx("R", &lem, &len, V(vpos), TAU(taupos), A(ed+1, st), &ldx, work);
        */
        magma_zlarfr_gpu_kernel<<< 1, BLOCK_SIZE >>>(lem, len, dV+vpos, dTAU+taupos, 
                                                     dA(ed+1, st), lddx);
    }
    if(lem>1){
        magma_bulge_findVTAUpos(n, nb, Vblksiz, sweep-1, ed, lddv, &vpos, &taupos);

        /* === Compute the following using one multiprocessor with BLOCK_SIZE threads ===

           // remove the first column of the created bulge 
           *V(vpos)  = c_one;
           cblas_zcopy(lem-1, A(ed+2, st), ione, V(vpos+1), ione);
           memset(A(ed+2, st),0,(lem-1)*sizeof(cuDoubleComplex));

           // Eliminate the col at st 
           lapackf77_zlarfg( &lem, A(ed+1, st), V(vpos+1), &ione, TAU(taupos) );

           // apply left on A(J1:J2,st+1:ed) 
           len = len-1; // because we start at col st+1 instead of st. col st is the col that has been removed
           conjtmp = MAGMA_Z_CNJG(*TAU(taupos));
           lapackf77_zlarfx("L", &lem, &len, V(vpos),  &conjtmp, A(ed+1, st+1), &ldx, work);
        */
        magma_ztrdtype2cbHLsym_withQ_v2_gpu_kernel<<<1, BLOCK_SIZE>>>(lem, len,
                                                                      dA, ldda,
                                                                      dV + vpos, dTAU + taupos,
                                                                      st, ed);
    }

}
#undef dA
#undef dAC

#undef dV
#undef dTAU

///////////////////////////////////////////////////////////
//                  TYPE 1-LPK Householder
///////////////////////////////////////////////////////////
//// add -1 because of C
#define A(i,j)   &(A[((i)-(j)) + lda*((j)-1)])
#define V(i)     &(V[(i)])
#define TAU(i)   &(TAU[(i)])
extern "C" void
magma_ztrdtype3cbHLsym_withQ_v2_fake(magma_int_t n, magma_int_t nb, cuDoubleComplex *A, magma_int_t lda, cuDoubleComplex *V, magma_int_t ldv, cuDoubleComplex *TAU,
                                magma_int_t st, magma_int_t ed, magma_int_t sweep, magma_int_t Vblksiz, cuDoubleComplex *work) {

    /*
     WORK (workspace) double complex array, dimension N
     */

    magma_int_t vpos, taupos;

    magma_bulge_findVTAUpos(n, nb, Vblksiz, sweep-1, st-1, ldv, &vpos, &taupos);

    magma_int_t len = ed-st+1;

    /* apply left and right on A(st:ed,st:ed)*/
    //magma_zlarfxsym_v2(len, A(st,st), lda-1, V(vpos), TAU(taupos), work);

}
#undef A
#undef V
#undef TAU
///////////////////////////////////////////////////////////





