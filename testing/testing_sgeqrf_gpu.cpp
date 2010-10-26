/*
    -- MAGMA (version 1.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2010
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

#define min(a,b)  (((a)<(b))?(a):(b))

extern "C" void sqrt02_(int *, int *, int *, float *, float *, float *,
                        float *, int *, float *, float *,
			int *, float *, float *);

/* ////////////////////////////////////////////////////////////////////////////
   -- Testing sgeqrf
*/
int main( int argc, char** argv) 
{
    cuInit( 0 );
    cublasInit( );
    printout_devices( );

    float *h_A, *h_R, *h_work, *tau, *d_A;
    float gpu_perf, cpu_perf;

    TimeStruct start, end;

    /* Matrix size */
    int M=0, N=0, n2, lda;
    int size[10] = {1024,2048,3072,4032,5184,6016,7040,8064,9088,10112};
    
    cublasStatus status;
    int i, j, info[1];

    if (argc != 1){
      for(i = 1; i<argc; i++){	
	if (strcmp("-N", argv[i])==0)
	  N = atoi(argv[++i]);
	else if (strcmp("-M", argv[i])==0)
          M = atoi(argv[++i]);
      }
      if (M>0 && N>0)
	printf("  testing_sgeqrf_gpu -M %d -N %d\n\n", M, N);
      else
	{
	  printf("\nUsage: \n");
	  printf("  testing_sgeqrf_gpu -M %d -N %d\n\n", 1024, 1024);
	  exit(1);
	}
    }
    else {
      printf("\nUsage: \n");
      printf("  testing_sgeqrf_gpu -M %d -N %d\n\n", 1024, 1024);
      M = N = size[9];
    }

    /* Initialize CUBLAS */
    status = cublasInit();
    if (status != CUBLAS_STATUS_SUCCESS) {
        fprintf (stderr, "!!!! CUBLAS initialization error\n");
    }

    lda = (M/32)*32;
    if (lda<M) lda+=32;

    n2  = M * N;

    int min_mn = min(M, N);

    /* Allocate host memory for the matrix */
    h_A = (float*)malloc(n2 * sizeof(h_A[0]));
    if (h_A == 0) {
        fprintf (stderr, "!!!! host memory allocation error (A)\n");
    }

    tau = (float*)malloc(min_mn * sizeof(float));
    if (tau == 0) {
      fprintf (stderr, "!!!! host memory allocation error (tau)\n");
    }
  
    cudaMallocHost( (void**)&h_R,  n2*sizeof(float) );
    if (h_R == 0) {
        fprintf (stderr, "!!!! host memory allocation error (R)\n");
    }

    int nb = magma_get_sgeqrf_nb(min_mn);
    int lwork = (M+N)*nb;

    status = cublasAlloc(lda*N, sizeof(float), (void**)&d_A);
    if (status != CUBLAS_STATUS_SUCCESS) {
      fprintf (stderr, "!!!! device memory allocation error (d_A)\n");
    }

    cudaMallocHost( (void**)&h_work, lwork*sizeof(float) );
    if (h_work == 0) {
      fprintf (stderr, "!!!! host memory allocation error (work)\n");
    }

    printf("\n\n");
    printf("  M     N   CPU GFlop/s   GPU GFlop/s    ||R||_F / ||A||_F\n");
    printf("==========================================================\n");
    for(i=0; i<10; i++){
      if (argc==1){
	M = N = min_mn = size[i];
        n2 = M*N;
	
	lda = (M/32)*32;
	if (lda<M) lda+=32;
      }

      for(j = 0; j < n2; j++)
	h_A[j] = rand() / (float)RAND_MAX;

      cublasSetMatrix( M, N, sizeof(float), h_A, M, d_A, lda);
      magma_sgeqrf_gpu( M, N, d_A, lda, tau, h_work, &lwork, info);
      cublasSetMatrix( M, N, sizeof(float), h_A, M, d_A, lda);

      /* ====================================================================
         Performs operation using MAGMA
	 =================================================================== */
      start = get_current_time();
      magma_sgeqrf_gpu( M, N, d_A, lda, tau, h_work, &lwork, info);
      end = get_current_time();
    
      gpu_perf = 4.*M*N*min_mn/(3.*1000000*GetTimerValue(start,end));
      // printf("GPU Processing time: %f (ms) \n", GetTimerValue(start,end));

      /* =====================================================================
         Performs operation using LAPACK 
	 =================================================================== */
      start = get_current_time();
      sgeqrf_(&M, &N, h_A, &M, tau, h_work, &lwork, info);
      end = get_current_time();
      if (info[0] < 0)  
	printf("Argument %d of sgeqrf had an illegal value.\n", -info[0]);
  
      cpu_perf = 4.*M*N*min_mn/(3.*1000000*GetTimerValue(start,end));
      // printf("CPU Processing time: %f (ms) \n", GetTimerValue(start,end));
      
      /* =====================================================================
         Check the result compared to LAPACK
         =================================================================== */
      cublasGetMatrix( M, N, sizeof(float), d_A, lda, h_R, M);
      
      float work[1], matnorm = 1., mone = -1.;
      int one = 1;
      matnorm = slange_("f", &M, &N, h_A, &M, work);
      saxpy_(&n2, &mone, h_A, &one, h_R, &one);
      printf("%5d %5d  %6.2f         %6.2f        %e\n", 
	     M, N, cpu_perf, gpu_perf,
	     slange_("f", &M, &N, h_R, &M, work) / matnorm);
      
      /* =====================================================================
         Check the factorization
         =================================================================== */
      /*
      float result[2];
      float *hwork_Q = (float*)malloc( M * N * sizeof(float));
      float *hwork_R = (float*)malloc( M * N * sizeof(float));
      float *rwork   = (float*)malloc( N * sizeof(float));

      sqrt02_(&M, &min_mn, &min_mn, h_A, h_R, hwork_Q, hwork_R, &lda, tau,
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
    cublasFree(h_R);
    cublasFree(d_A);

    /* Shutdown */
    status = cublasShutdown();
    if (status != CUBLAS_STATUS_SUCCESS) {
        fprintf (stderr, "!!!! shutdown error (A)\n");
    }
}
