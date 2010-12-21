/*
 *  -- MAGMA (version 1.0) --
 *     Univ. of Tennessee, Knoxville
 *     Univ. of California, Berkeley
 *     Univ. of Colorado, Denver
 *     November 2010
 *
 *  @precisions normal z -> c d s
 *
 **/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cublas.h>
#include <cblas.h>

#include "flops.h"
#include "magma.h"
#include "testings.h"

#define assert( check, ... ) { if( !(check) ) { fprintf(stderr, __VA_ARGS__ ); exit(-1); } }

#define PRECISION_z
#if defined(PRECISION_z) || defined(PRECISION_c)
#define FLOPS(n) ( 6. * FMULS_SYMV(n) + 2. * FADDS_SYMV(n))
#else
#define FLOPS(n) (      FMULS_SYMV(n) +      FADDS_SYMV(n))
#endif

int main(int argc, char **argv)
{	
    CUdevice  dev;
    CUcontext context;
    FILE *fp ; 
    TimeStruct start, end;
    int N, m;
    int matsize;
    int vecsize;
    int ione     = 1;
    int ISEED[4] = {0,0,0,1};
    int st       = 64;
    int incx     = 1;
    cuDoubleComplex mzone = MAGMA_Z_NEG_ONE;
    double  work[1];
    double  res;
    char uplo = MagmaLower;
    
    fp = fopen ("results_zsymv.txt", "w") ;
    if( fp == NULL ){ printf("Couldn't open output file\n"); exit(1);}

    printf("SYMV cuDoubleComplex Precision\n\n"
           "Usage\n\t\t testing_zsymv U|L N\n\n");
    fprintf(fp, "SYMV cuDoubleComplex Precision\n\n"
            "Usage\n\t\t testing_zsymv U|L N\n\n");
    
    assert( CUDA_SUCCESS == cuInit( 0 ),
            "CUDA: Not initialized\n" );
    assert( CUDA_SUCCESS == cuDeviceGet( &dev, 0 ),
            "CUDA: Cannot get the device\n");
    assert( CUDA_SUCCESS == cuCtxCreate( &context, 0, dev ),
            "CUDA: Cannot create the context\n");
    assert( CUDA_SUCCESS == cublasInit( ),
            "CUBLAS: Not initialized\n" );
	
    printout_devices( );
	

    N = 8*1024+64;
    if( argc > 1 ) {
      uplo = argv[1][0];
    }
    if( argc > 2 ) {
      st = N = atoi( argv[2] );
    }
    matsize = N*N;
    vecsize = N*incx;

    cuDoubleComplex *A = (cuDoubleComplex*)malloc( matsize*sizeof( cuDoubleComplex ) );
    cuDoubleComplex *X = (cuDoubleComplex*)malloc( vecsize*sizeof( cuDoubleComplex ) );
    cuDoubleComplex *Y = (cuDoubleComplex*)malloc( vecsize*sizeof( cuDoubleComplex ) );
    
    assert( ((A != NULL) && (X != NULL) && (Y != NULL)), "memory allocation error (A, X or Y)" );
    
    lapackf77_zlarnv( &ione, ISEED, &matsize, A );
    /* Make A symmetric */
    { 
        int i, j;
        for(i=0; i<N; i++) {
            for(j=0; j<i; j++)
                A[i*N+j] = A[j*N+i];
        }
    }
    lapackf77_zlarnv( &ione, ISEED, &vecsize, X );
    lapackf77_zlarnv( &ione, ISEED, &vecsize, Y );
	
    cuDoubleComplex *Ycublas = (cuDoubleComplex*)malloc(vecsize*sizeof( cuDoubleComplex ) );
    cuDoubleComplex *Ymagma  = (cuDoubleComplex*)malloc(vecsize*sizeof( cuDoubleComplex ) );
	
    assert( ((Ycublas != NULL) && (Ymagma != NULL)), "memory allocation error (Y cublas or magma)" );
    
    cuDoubleComplex *dA, *dX, *dY;
    
    assert( CUBLAS_STATUS_SUCCESS == cublasAlloc( matsize, sizeof(cuDoubleComplex), (void**)&dA ),
            "CUBLAS: Problem of allocation of dA\n" );
    assert( CUBLAS_STATUS_SUCCESS == cublasAlloc( vecsize, sizeof(cuDoubleComplex), (void**)&dX ),
            "CUBLAS: Problem of allocation of dX\n" );
    assert( CUBLAS_STATUS_SUCCESS == cublasAlloc( vecsize, sizeof(cuDoubleComplex), (void**)&dY ),
            "CUBLAS: Problem of allocation of dY\n" );

    printf( "   n   CUBLAS,Gflop/s   MAGMABLAS0.2,Gflop/s   \"error\"\n" 
            "==============================================================\n");
    fprintf(fp, "   n   CUBLAS,Gflop/s   MAGMABLAS0.2,Gflop/s   \"error\"\n" 
            "==============================================================\n");
    
    for( m = st; m < N+1; m = (int)((m+1)*1.1) )
    {
        int lda  = m;
        cuDoubleComplex alpha = MAGMA_Z_MAKE(  1.5, -2.3 );
        cuDoubleComplex beta  = MAGMA_Z_MAKE( -0.6,  0.8 );
        double time, gflops;
        double flops = FLOPS( (double)m ) / 1e6;

        printf(      "%5d ", m );
        fprintf( fp, "%5d ", m );

        /* =====================================================================
           Performs operation using CUDA-BLAS
           =================================================================== */
        cublasSetMatrix( m, m, sizeof( cuDoubleComplex ), A, N,    dA, lda  );
        cublasSetVector( m,    sizeof( cuDoubleComplex ), X, incx, dX, incx );
        cublasSetVector( m,    sizeof( cuDoubleComplex ), Y, incx, dY, incx );

        /*
         * Cublas doesn't define cublasZsymv and cublasCsymv
         */
        gflops = 0.0;
#if defined(PRECISION_d) || defined(PRECISION_s)
        cublasZsymv( uplo, m, alpha, dA, lda, dX, incx, beta, dY, incx );
        cublasSetVector( m, sizeof( cuDoubleComplex ), Y, incx, dY, incx );
        
        start = get_current_time();
        cublasZsymv( uplo, m, alpha, dA, lda, dX, incx, beta, dY, incx );
        end = get_current_time();
        time = GetTimerValue(start,end); 
        
        cublasGetVector( m, sizeof( cuDoubleComplex ), dY, incx, Ycublas, incx );
        
        gflops = flops / time;
#endif
        printf(     "%11.2f", gflops );
        fprintf(fp, "%11.2f", gflops );

        cublasSetVector( m, sizeof( cuDoubleComplex ), Y, incx, dY, incx );
        magmablas_zsymv( uplo, m, alpha, dA, lda, dX, incx, beta, dY, incx );
        cublasSetVector( m, sizeof( cuDoubleComplex ), Y, incx, dY, incx );
        
        start = get_current_time();
        magmablas_zsymv( uplo, m, alpha, dA, lda, dX, incx, beta, dY, incx );
        end = get_current_time();
        time = GetTimerValue(start,end) ; 
        
        cublasGetVector( m, sizeof( cuDoubleComplex ), dY, incx, Ymagma, incx );
        
        gflops = flops / time;
        printf(     "%11.2f", gflops );
        fprintf(fp, "%11.2f", gflops );

        /* =====================================================================
           Computing the Difference Cublas VS Magma
           =================================================================== */
        
#if defined(PRECISION_z) || defined(PRECISION_c)
        /*
         * In case on complex, we use cblas to compare
         */
        cblas_zcopy( m, Y, incx, Ycublas, incx );
        lapackf77_zsymv( MagmaLowerStr, &m, &alpha, A, &N, X, &incx, &beta, Ycublas, &incx );
#endif

        blasf77_zaxpy( &m, &mzone, Ymagma, &incx, Ycublas, &incx);
        res = lapackf77_zlange( "M", &m, &ione, Ycublas, &m, work );

        printf(      "\t\t %8.6e\n", res / m );
        fprintf( fp, "\t\t %8.6e\n", res / m );
    }
    
    free( A );
    free( X );
    free( Y );
    free( Ycublas );
    free( Ymagma );
    
    cudaFree( dA );
    cudaFree( dX );
    cudaFree( dY );
     
    fclose( fp ) ; 
    cuCtxDetach( context );
    cublasShutdown();
    
    return 0;
}	
