/*
 *  -- MAGMA (version 1.0) --
 *     Univ. of Tennessee, Knoxville
 *     Univ. of California, Berkeley
 *     Univ. of Colorado, Denver
 *     November 2010
 *
 * @precisions normal z -> c d s
 *
 **/
// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cublas.h>

#define CHECK
// includes, project
#include "magma.h"
#include "magmablas.h"

#define PRECISION_z
#if 0
static void printMatrix( cuDoubleComplex *A, int m, int n, int lda){
    char name[256];
    FILE *file = fopen("toto.mtx", "w");
    int i, j;

#if defined(PRECISION_d) || defined(PRECISION_s)
    fprintf(stderr, "----------------------------------------\n");
    for(i=0; i<m; i++) {
	for(j=0; j<n; j++)
	    fprintf(stderr, "%e ", A[lda*j+i]);
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "----------------------------------------\n\n");
#endif
}
#endif

#ifdef CHECK
static int diffMatrix( cuDoubleComplex *A, cuDoubleComplex *B, int m, int n, int lda){
    int i, j;
    for(i=0; i<m; i++) {
	for(j=0; j<n; j++)
	    if ( !(MAGMA_Z_EQUAL( A[lda*j+i], B[lda*j+i] )) )
                return 0;
    }
    return 1;
}
#endif
/* ////////////////////////////////////////////////////////////////////////////
   -- Testing zpotrf
*/
int main( int argc, char** argv) 
{
    cuInit( 0 );
    cublasInit( );
    printout_devices( );

    double2 *h_A1, *h_A2;
    double2 *d_A1, *d_A2;
    double gpu_perf, cpu_perf;
    double gpu_perf2, cpu_perf2;
    double gpu_perf3, cpu_perf3;
    double gpu_perfx;

    TimeStruct start, end;

    /* Matrix size */
    int N=64, n2, lda;
    int size[10] = {1024,2048,3072,4032,5184,6048,7200,8064,8928,10080};
    cublasStatus status;
    int i, j;
    int ione     = 1;
    int ISEED[4] = {0,0,0,1};
    int *ipiv;
    
    if (argc != 1){
      for(i = 1; i<argc; i++){	
	if (strcmp("-N", argv[i])==0)
	  N = atoi(argv[++i]);
      }
      if (N>0) size[0] = size[9] = N;
      else exit(1);
    }
    else {
      printf("\nUsage: \n");
      printf("  testing_zswap -N %d\n\n", 1024);
    }

    /* Initialize CUBLAS */
    status = cublasInit();
    if (status != CUBLAS_STATUS_SUCCESS) {
        fprintf (stderr, "!!!! CUBLAS initialization error\n");
    }

    N = lda = size[9];
    n2 = N*N;

    /* Allocate host memory for the matrix */
    cudaMallocHost((void**)(&h_A1), n2 * sizeof(double2));
    if (h_A1 == 0) {
        fprintf (stderr, "!!!! host memory allocation error (A)\n");
    }
    
    cudaMallocHost((void**)(&h_A2), n2 * sizeof(double2));
    if (h_A2 == 0) {
        fprintf (stderr, "!!!! host memory allocation error (A)\n");
    }
  
    ipiv = (int*)malloc(N * sizeof(int));
    if (ipiv == 0) {
        fprintf (stderr, "!!!! host memory allocation error (ipiv)\n");
    }
  
    status = cublasAlloc(n2+32*N, sizeof(double2), (void**)&d_A1);
    if (status != CUBLAS_STATUS_SUCCESS) {
      fprintf (stderr, "!!!! device memory allocation error (d_A)\n");
    }
    status = cublasAlloc(n2+32*N, sizeof(double2), (void**)&d_A2);
    if (status != CUBLAS_STATUS_SUCCESS) {
      fprintf (stderr, "!!!! device memory allocation error (d_A)\n");
    }

#ifdef CHECK2        
    for(i=0; i<N; i++) {
	for(j=0; j<N; j++)
            h_A1[lda*j+i] = MAGMA_Z_MAKE( (double)(i), 0.);
    }
    for(i=0; i<N; i++) {
	for(j=0; j<N; j++)
            h_A2[lda*j+i] = MAGMA_Z_MAKE( (double)(n2+i), 0.);
    }
#else
     lapackf77_zlarnv( &ione, ISEED, &n2, h_A1 );
     lapackf77_zlarnv( &ione, ISEED, &n2, h_A2 );
#endif

    for(i=0; i<N; i++) {
        ipiv[i] = (int)((rand()*1.*N) / (RAND_MAX * 1.)) + 1;
#ifdef CHECK
        fprintf(stderr, "%d\n", ipiv[i]);
#endif
    }

#ifdef CHECK        
    double2 *h_A3, *h_A4;
    cudaMallocHost((void**)(&h_A3), n2 * sizeof(double2));
    if (h_A3 == 0) {
        fprintf (stderr, "!!!! host memory allocation error (A)\n");
    }
    
    cudaMallocHost((void**)(&h_A4), n2 * sizeof(double2));
    if (h_A4 == 0) {
        fprintf (stderr, "!!!! host memory allocation error (A)\n");
    }

    memcpy(h_A3, h_A1, n2 * sizeof(double2));
    memcpy(h_A4, h_A2, n2 * sizeof(double2));
#endif

    /* Initialize the matrix */
    cublasSetMatrix( N, N, sizeof(double2), h_A1, lda, d_A1, lda);
    cublasSetMatrix( N, N, sizeof(double2), h_A2, lda, d_A2, lda);
    
    printf("\n\n");
    printf("  N         CM V1     RM V1    CM V2     RM V2      CM LAP RM      OLD LAP  GFlop/s    \n");
    printf("=======================================================================================\n");
    for(i=0; i<10; i++) {
        N = lda = size[i];
        n2 = N*N;

        /*
         * Version 1 of BLAS swap
         */
        /* Row Major */
        start = get_current_time();
        for ( j=0; j<N; j++) {
            if ( j != (ipiv[j]-1)) {
                magmablas_zswap( N, d_A1+lda*j, 1, d_A2+lda*(ipiv[j]-1), 1);
            }
        }
        end = get_current_time();
        gpu_perf = 1.*N*N/(1000000.*GetTimerValue(start,end));
        
#ifdef CHECK        
        for ( j=0; j<N; j++) {
            if ( j != (ipiv[j]-1)) {
              blasf77_zswap( &N, h_A1+lda*j, &ione, h_A2+lda*(ipiv[j]-1), &ione);
            }
        }
        cublasGetMatrix( N, N, sizeof(double2), d_A1, N, h_A2, N);
        if ( diffMatrix( h_A1, h_A2, N, N, N ) == 1 ) {
            fprintf(stderr, "Check BLAS swap RM: FAILED\n");
        } else {
            fprintf(stderr, "Check BLAS swap RM: PASSED\n");
        }
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A3, size[9], d_A1, size[9]);
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A4, size[9], d_A2, size[9]);
#endif

        /* Column Major */
        start = get_current_time();
        for ( j=0; j<N; j++) {
            if ( j != (ipiv[j]-1)) {
                magmablas_zswap( N, d_A1+j, lda, d_A2+ipiv[j]-1, lda);
            }
        }
        end = get_current_time();
        cpu_perf = 1.*N*N/(1000000.*GetTimerValue(start,end));
        
#ifdef CHECK        
        for ( j=0; j<N; j++) {
            if ( j != (ipiv[j]-1)) {
              blasf77_zswap( &N, h_A1+lda*j, &lda, h_A2+lda*(ipiv[j]-1), &lda);
            }
        }
        cublasGetMatrix( N, N, sizeof(double2), d_A1, N, h_A2, N);
        if ( diffMatrix( h_A1, h_A2, N, N, N ) == 1 ) {
            fprintf(stderr, "Check BLAS swap CM: FAILED\n");
        } else {
            fprintf(stderr, "Check BLAS swap CM: PASSED\n");
        }
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A3, size[9], d_A1, size[9]);
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A4, size[9], d_A2, size[9]);
#endif

        /*
         * Version 2 of BLAS swap
         */

        /* Row Major */
        start = get_current_time();
        magmablas_zswapblk( N, d_A1, lda, 1, d_A2, lda, 1, 1, N, ipiv, 1, 0);
        end = get_current_time();
        gpu_perf2 = 1.*N*N/(1000000.*GetTimerValue(start,end));
        
#ifdef CHECK        
        for ( j=0; j<N; j++) {
            if ( j != (ipiv[j]-1)) {
              blasf77_zswap( &N, h_A1+lda*j, &ione, h_A2+lda*(ipiv[j]-1), &ione);
            }
        }
        cublasGetMatrix( N, N, sizeof(double2), d_A1, N, h_A2, N);
        if ( diffMatrix( h_A1, h_A2, N, N, N ) == 1 ) {
            fprintf(stderr, "Check BLASblk swap RM: FAILED\n");
        } else {
            fprintf(stderr, "Check BLASblk swap RM: PASSED\n");
        }
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A3, size[9], d_A1, size[9]);
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A4, size[9], d_A2, size[9]);
#endif

        /* Column Major */
        start = get_current_time();
        magmablas_zswapblk( N, d_A1, 1, lda, d_A2, 1, lda, 1, N, ipiv, 1, 0);
        end = get_current_time();
        cpu_perf2 = 1.*N*N/(1000000.*GetTimerValue(start,end));
        
#ifdef CHECK        
        for ( j=0; j<N; j++) {
            if ( j != (ipiv[j]-1)) {
              blasf77_zswap( &N, h_A1+lda*j, &lda, h_A2+lda*(ipiv[j]-1), &lda);
            }
        }
        cublasGetMatrix( N, N, sizeof(double2), d_A1, N, h_A2, N);
        if ( diffMatrix( h_A1, h_A2, N, N, N ) == 1 ) {
            fprintf(stderr, "Check BLASblk swap CM: FAILED\n");
        } else {
            fprintf(stderr, "Check BLASblk swap CM: PASSED\n");
        }
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A3, size[9], d_A1, size[9]);
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A4, size[9], d_A2, size[9]);
#endif
        /*
         * Version 1 of LAPACK swap (Old one)
         */
        start = get_current_time();
        magmablas_zlaswp( N, d_A1, N, 1, N, ipiv, 1);
        end = get_current_time();
        gpu_perfx = 1.*N*N/(1000000.*GetTimerValue(start,end));

#ifdef CHECK        
        lapackf77_zlaswp( &N, h_A1, &N, &ione, &N, ipiv, &ione);
        cublasGetMatrix( N, N, sizeof(double2), d_A1, N, h_A2, N);
        if ( diffMatrix( h_A1, h_A2, N, N, N ) == 1 ) {
            fprintf(stderr, "Check OldLapack swap RM: FAILED\n");
        } else {
            fprintf(stderr, "Check OldLapack swap RM: PASSED\n");
        }
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A3, size[9], d_A1, size[9]);
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A4, size[9], d_A2, size[9]);
#endif

        /* Row Major */
        start = get_current_time();
        magmablas_zlaswpx( N, d_A1, N, 1, 1, N, ipiv, 1);
        end = get_current_time();
        gpu_perf3 = 1.*N*N/(1000000.*GetTimerValue(start,end));

#ifdef CHECK        
        lapackf77_zlaswp( &N, h_A1, &N, &ione, &N, ipiv, &ione);
        cublasGetMatrix( N, N, sizeof(double2), d_A1, N, h_A2, N);
        if ( diffMatrix( h_A1, h_A2, N, N, N ) == 1 ) {
            fprintf(stderr, "Check Laswp RM: FAILED\n");
        } else {
            fprintf(stderr, "Check Laswp RM: PASSED\n");
        }
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A3, size[9], d_A1, size[9]);
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A4, size[9], d_A2, size[9]);
#endif

        /* Col Major */
        start = get_current_time();
        magmablas_zlaswpx( N, d_A1, 1, N, 1, N, ipiv, 1);
        end = get_current_time();
        cpu_perf3 = 1.*N*N/(1000000.*GetTimerValue(start,end));

#ifdef CHECK        
        for ( j=0; j<N; j++) {
            if ( j != (ipiv[j]-1)) {
              blasf77_zswap( &N, h_A1+lda*j, &lda, h_A1+lda*(ipiv[j]-1), &lda);
            }
        }
        cublasGetMatrix( N, N, sizeof(double2), d_A1, N, h_A2, N);
        if ( diffMatrix( h_A1, h_A2, N, N, N ) == 1 ) {
            fprintf(stderr, "Check Laswp RM: FAILED\n");
        } else {
            fprintf(stderr, "Check Laswp RM: PASSED\n");
        }
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A3, size[9], d_A1, size[9]);
        cublasSetMatrix( size[9], size[9], sizeof(double2), h_A4, size[9], d_A2, size[9]);
#endif

        printf("%5d      %6.2f / %6.2f    %6.2f / %6.2f   %6.2f / %6.2f   %6.2f \n", 
               size[i], cpu_perf, gpu_perf, cpu_perf2, gpu_perf2, cpu_perf3, gpu_perf3, gpu_perfx);

      if (argc != 1)
	break;
    }
    
    /* Memory clean up */
    cudaFreeHost(h_A1);
    cudaFreeHost(h_A2);
    cublasFree(d_A1);
    cublasFree(d_A2);

    /* Shutdown */
    status = cublasShutdown();
    if (status != CUBLAS_STATUS_SUCCESS) {
        fprintf (stderr, "!!!! shutdown error (A)\n");
    }
}
