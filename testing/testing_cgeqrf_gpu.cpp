/*
    -- MAGMA (version 0.2) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2009
*/

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// includes, project
#include "cuda.h"
#include "cuda_runtime_api.h"
#include "cublas.h"
#include "magma.h"

/* ////////////////////////////////////////////////////////////////////////////
   -- Testing cgeqrf
*/
int main( int argc, char** argv) 
{
    cuInit( 0 );
    cublasInit( );
    printout_devices( );

    float2 *h_A, *h_R, *h_work, *tau;
    float2 *d_A, *d_work;
    float gpu_perf, cpu_perf;

    TimeStruct start, end;

    /* Matrix size */
    int N=0, n2, lda;
    int size[10] = {1024,2048,3072,4032,5184,6016,7040,8064,9088,10112};
    
    cublasStatus status;
    int i, j, info[1];

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
      printf("  testing_cgeqrf_gpu -N %d\n\n", 1024);
    }

    /* Initialize CUBLAS */
    status = cublasInit();
    if (status != CUBLAS_STATUS_SUCCESS) {
        fprintf (stderr, "!!!! CUBLAS initialization error\n");
    }

    lda = N;
    n2 = size[9] * size[9];

    /* Allocate host memory for the matrix */
    h_A = (float2*)malloc(n2 * sizeof(h_A[0]));
    if (h_A == 0) {
        fprintf (stderr, "!!!! host memory allocation error (A)\n");
    }

    tau = (float2*)malloc(size[9] * sizeof(float2));
    if (tau == 0) {
      fprintf (stderr, "!!!! host memory allocation error (tau)\n");
    }
  
    cudaMallocHost( (void**)&h_R,  n2*sizeof(float2) );
    if (h_R == 0) {
        fprintf (stderr, "!!!! host memory allocation error (R)\n");
    }

    int lwork = 2*size[9]*magma_get_cgeqrf_nb(size[9]);
    status = cublasAlloc(n2, sizeof(float2), (void**)&d_A);
    if (status != CUBLAS_STATUS_SUCCESS) {
      fprintf (stderr, "!!!! device memory allocation error (d_A)\n");
    }

    status = cublasAlloc(lwork/2, sizeof(float2), (void**)&d_work);
    if (status != CUBLAS_STATUS_SUCCESS) {
      fprintf (stderr, "!!!! device memory allocation error (d_work)\n");
    }

    cudaMallocHost( (void**)&h_work, lwork*sizeof(float2) );
    if (h_work == 0) {
      fprintf (stderr, "!!!! host memory allocation error (work)\n");
    }

    printf("\n\n");
    printf("  N    CPU GFlop/s    GPU GFlop/s    ||R||_F / ||A||_F\n");
    printf("========================================================\n");
    for(i=0; i<10; i++){
      N = lda = size[i];
      n2 = N*N;

      for(j = 0; j < n2; j++)
	h_A[j].x = rand() / (float)RAND_MAX; h_A[j].y = rand() / (float)RAND_MAX;

      cublasSetVector(n2, sizeof(float2), h_A, 1, d_A, 1);
      magma_cgeqrf_gpu(&N, &N, d_A, &N, tau, h_work, &lwork, d_work, info);
      cublasSetVector(n2, sizeof(float2), h_A, 1, d_A, 1);
  
      /* ====================================================================
         Performs operation using MAGMA
	 =================================================================== */
      start = get_current_time();
      magma_cgeqrf_gpu(&N, &N, d_A,&N, tau, h_work, &lwork, d_work, info);
      end = get_current_time();
    
      gpu_perf = 4.*4.*N*N*N/(3.*1000000*GetTimerValue(start,end));
      // printf("GPU Processing time: %f (ms) \n", GetTimerValue(start,end));

      /* =====================================================================
         Performs operation using LAPACK 
	 =================================================================== */
      start = get_current_time();
      cgeqrf_(&N, &N, h_A, &lda, tau, h_work, &lwork, info);
      end = get_current_time();
      if (info[0] < 0)  
	printf("Argument %d of cgeqrf had an illegal value.\n", -info[0]);
  
      cpu_perf = 4.*4.*N*N*N/(3.*1000000*GetTimerValue(start,end));
      // printf("CPU Processing time: %f (ms) \n", GetTimerValue(start,end));
      
      /* =====================================================================
         Check the result compared to LAPACK
         =================================================================== */
      cublasGetVector(n2, sizeof(float2), d_A, 1, h_R, 1);
      
      float work[1]; 
      float matnorm = 1.; 
      float2 mone = {-1., 0.};
      int one = 1;
      matnorm = clange_("f", &N, &N, h_A, &N, work);
      caxpy_(&n2, &mone, h_A, &one, h_R, &one);
      printf("%5d    %6.2f         %6.2f        %e\n", 
	     size[i], cpu_perf, gpu_perf,
	     clange_("f", &N, &N, h_R, &N, work) / matnorm);
      /* =====================================================================
         Check the factorization
         =================================================================== */
      /*
      float result[2];
      float2 *hwork_Q = (float2*)malloc( N * N * sizeof(float2));
      float2 *hwork_R = (float2*)malloc( N * N * sizeof(float2));
      float2 *rwork   = (float2*)malloc( N * sizeof(float2));

      sqrt02(&N, &N, &N, h_A, h_R, hwork_Q, hwork_R, &N, tau,
             h_work, &lwork, rwork, result);

      printf("norm( R - Q'*A ) / ( M * norm(A) * EPS ) = %f\n", result[0]);
      printf("norm( I - Q'*Q ) / ( M * EPS )           = %f\n", result[1]);
      free(hwork_Q);
      free(hwork_R);
      free(rwork);
      */

      if (argc != 1)
	break;
    }

    /* Memory clean up */
    free(h_A);
    free(tau);
    cublasFree(h_work);
    cublasFree(d_work);
    cublasFree(h_R);
    cublasFree(d_A);

    /* Shutdown */
    status = cublasShutdown();
    if (status != CUBLAS_STATUS_SUCCESS) {
        fprintf (stderr, "!!!! shutdown error (A)\n");
    }
}
