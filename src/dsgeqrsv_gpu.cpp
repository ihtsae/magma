#include <stdio.h>
#include <math.h>
#include "magmablas.h"
#include "magma.h"
#include "cublas.h"
#include "cuda.h"
#include "cuda.h"
#include "cuda_runtime_api.h"
#include "cublas.h"
#include "magma.h"

#define BWDMAX 1.0
#define ITERMAX 30

extern "C" int
magma_dsgeqrsv_gpu(int M, int N, int NRHS, double *A, int LDA, double *B, 
		   int LDB, double *X,int LDX, double *WORK, float *SWORK, 
		   int *ITER, int *INFO, float *tau, int lwork, float *h_work,
		   float *d_work, double *tau_d, int lwork_d, double *h_work_d,
		   double *d_work_d)
{
/*  -- MAGMA (version 1.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2010

    Purpose
    =======

    DSGEQRSV solves the least squares problem 
       min || A*X - B ||,
    where A is an M-by-N matrix and X and B are M-by-NRHS matrices.

    DSGEQRSV first attempts to factorize the matrix in SINGLE PRECISION
    and use this factorization within an iterative refinement procedure
    to produce a solution with DOUBLE PRECISION norm-wise backward error
    quality (see below). If the approach fails the method switches to a
    DOUBLE PRECISION factorization and solve.

    The iterative refinement is not going to be a winning strategy if
    the ratio SINGLE PRECISION performance over DOUBLE PRECISION
    performance is too small. A reasonable strategy should take the
    number of right-hand sides and the size of the matrix into account.
    This might be done with a call to ILAENV in the future. Up to now, we
    always try iterative refinement.
    The iterative refinement process is stopped if
        ITER > ITERMAX
    or for all the RHS we have:
        RNRM < SQRT(N)*XNRM*ANRM*EPS*BWDMAX
    where
        o ITER is the number of the current iteration in the iterative
          refinement process
        o RNRM is the infinity-norm of the residual
        o XNRM is the infinity-norm of the solution
        o ANRM is the infinity-operator-norm of the matrix A
        o EPS is the machine epsilon returned by DLAMCH('Epsilon')
    The value ITERMAX and BWDMAX are fixed to 30 and 1.0D+00 respectively.

    Arguments
    =========

    M       (input) INTEGER   
            The number of rows of the matrix A. M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix A. M >= N >= 0.

    NRHS    (input) INTEGER
            The number of right hand sides, i.e., the number of columns
            of the matrix B.  NRHS >= 0.

    A       (input or input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the M-by-N coefficient matrix A.
            On exit, if iterative refinement has been successfully used
            (INFO.EQ.0 and ITER.GE.0, see description below), A is
            unchanged. If double precision factorization has been used
            (INFO.EQ.0 and ITER.LT.0, see description below), then the
            array A contains the QR factorization of A as returned by
            function DGEQRF_GPU2.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,M).

    B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS)
            The M-by-NRHS right hand side matrix B.

    LDB     (input) INTEGER
            The leading dimension of the array B.  LDB >= max(1,M).

    X       (output) DOUBLE PRECISION array, dimension (LDX,NRHS)
            If INFO = 0, the N-by-NRHS solution matrix X.

    LDX     (input) INTEGER
            The leading dimension of the array X.  LDX >= max(1,N).

    WORK    (workspace) DOUBLE PRECISION array, dimension (N*NRHS)
            This array is used to hold the residual vectors.

    SWORK   (workspace) REAL array, dimension (M*(N+NRHS))
            This array is used to store the single precision matrix and the
            right-hand sides or solutions in single precision.

    ITER    (output) INTEGER
            < 0: iterative refinement has failed, double precision
                 factorization has been performed
                 -1 : the routine fell back to full precision for
                      implementation- or machine-specific reasons
                 -2 : narrowing the precision induced an overflow,
                      the routine fell back to full precision
                 -3 : failure of SGETRF
                 -31: stop the iterative refinement after the 30th
                      iterations
            > 0: iterative refinement has been successfully used.
                 Returns the number of iterations
 
    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    TAU     (output) REAL array, dimension (N)
            On exit, TAU(i) contains the scalar factor of the elementary
            reflector H(i), as returned by magma_sgeqrf_gpu2.

    LWORK   (input) INTEGER   
            The dimension of the array H_WORK.  LWORK >= (M+N+NB)*NB,   
            where NB can be obtained through magma_get_sgeqrf_nb(M).

    H_WORK  (workspace/output) REAL array, dimension (MAX(1,LWORK))   
            Higher performance is achieved if H_WORK is in pinned memory, e.g.
            allocated using cudaMallocHost.

    D_WORK  (workspace/output)  REAL array on the GPU, dimension 2*N*NB,
            where NB can be obtained through magma_get_sgeqrf_nb(M).
            It starts with NB*NB blocks that store the triangular T 
            matrices, followed by the NB*NB blocks of the diagonal 
            inverses for the R matrix.

    TAU_D   (output) DOUBLE REAL array, dimension (N)
            On exit, if the matrix had to be factored in double precision,
            TAU(i) contains the scalar factor of the elementary
            reflector H(i), as returned by magma_dgeqrf_gpu2.

    LWORK_D (input) INTEGER   
            The dimension of the array H_WORK_D. LWORK_D >= (M+N+NB)*NB,   
            where NB can be obtained through magma_get_dgeqrf_nb(M).

    H_WORK_D (workspace/output) DOUBLE REAL array, dimension (MAX(1,LWORK_D))
            This memory is unattached if the iterative refinement worked, 
            otherwise it is used as workspace to factor the matrix in
            double precision. Higher performance is achieved if H_WORK_D is 
            in pinned memory, e.g. allocated using cudaMallocHost. 

    D_WORK_D (workspace/output) DOUBLE REAL array on the GPU, dimension 2*N*NB,
            where NB can be obtained through magma_get_dgeqrf_nb(M).
            This memory is unattached if the iterative refinement worked, 
            otherwise it is used as workspace to factor the matrix in
            double precision. It starts with NB*NB blocks that store the 
            triangular T matrices, followed by the NB*NB blocks of the 
            diagonal inverses for the R matrix.

    =====================================================================    */

  #define max(a,b)       (((a)>(b))?(a):(b))

  /*
    Check The Parameters. 
  */
  *ITER = 0 ;
  *INFO = 0 ;
  if ( N <0)
    *INFO = -1;
  else if(NRHS<0)
    *INFO =-3;
  else if(LDA < max(1,N))
    *INFO =-5;
  else if( LDB < max(1,N))
    *INFO =-7;
  else if( LDX < max(1,N))
    *INFO =-9;

  if(*INFO!=0){
    printf("%d %d %d\n", M , N , NRHS);
    magma_xerbla("magma_dsgeqrsv_gpu",INFO) ;
  }

  if( N == 0 || NRHS == 0 )
    return 0;

  double ANRM , CTE , EPS;

  EPS  = dlamch_("Epsilon");
  ANRM = magma_dlange('I', N, N , A, LDA , WORK );
  CTE = ANRM * EPS *  pow((double)N,0.5) * BWDMAX ;
  int PTSA  = N*NRHS;
  float RMAX = slamch_("O");
  int IITER ;
  double alpha = -1.0;
  double beta = 1 ;
  magmablas_dlag2s(N , NRHS , B , LDB , SWORK, N , RMAX );
  if(*INFO !=0){
    *ITER = -2 ;
    printf("magmablas_dlag2s\n");
    goto L40;
  }
  magmablas_dlag2s(N , N , A , LDA , SWORK+PTSA, N , RMAX); // Merge with DLANGE /
  if(*INFO !=0){
    *ITER = -2 ;
    printf("magmablas_dlag2s\n");
    goto L40;
  }
  double XNRM[1] , RNRM[1] ;

  // In an ideal version these variables should come from user.
  magma_sgeqrf_gpu2(M, N, SWORK+PTSA, N, tau, d_work, INFO);

  if(INFO[0] !=0){
    *ITER = -3 ;
    goto L40;
  }

  // SWORK = B 
  magma_sgeqrs_gpu(M, N, NRHS, SWORK+PTSA, N, tau, SWORK, M, 
		   h_work, &lwork, d_work, INFO);

  // SWORK = X in SP 
  magmablas_slag2d(N, NRHS, SWORK, N, X, LDX, INFO);

  // X = X in DP 
  magma_dlacpy(N, NRHS, B , LDB, WORK, N);

  // WORK = B in DP; WORK contains the residual ...
  if( NRHS == 1 )
    magmablas_dgemv_MLU(N, N, A, LDA, X, WORK);
  else
    cublasDgemm('N', 'N', N, NRHS, N, -1.0, A, LDA, X, LDX, 1.0, WORK, N);

  int i,j;
  for(i=0;i<NRHS;i++){
    j = cublasIdamax( N ,X+i*N, 1) ;
    cublasGetMatrix( 1, 1, sizeof(double), X+i*N+j-1, 1,XNRM, 1 ) ;
    XNRM[0]= fabs( XNRM[0]);
    j = cublasIdamax ( N , WORK+i*N  , 1 ) ;
    cublasGetMatrix( 1, 1, sizeof(double), WORK+i*N+j-1, 1, RNRM, 1 ) ;
    RNRM[0] =fabs( RNRM[0]);
    if( RNRM[0] > XNRM[0]*CTE ){
      goto L10;
    }
  }
  
  *ITER =0;
  return 0;

 L10:
  ;

  for(IITER=1;IITER<ITERMAX;){
    *INFO = 0 ;
    /*  Convert R (in WORK) from double precision to single precision
        and store the result in SX.
        Solve the system SA*SX = SR.
        -- These two Tasks are merged here. */
    // make SWORK = WORK ... residuals... 
    magmablas_dlag2s(N , NRHS, WORK, LDB, SWORK, N, RMAX);
    magma_sgeqrs_gpu(M, N, NRHS, SWORK+PTSA, N, tau, SWORK,
		     M, h_work, &lwork, d_work, INFO);
    if(INFO[0] !=0){
      *ITER = -3 ;
      goto L40;
    }
    for(i=0;i<NRHS;i++){
       magmablas_sdaxpycp(SWORK+i*N,X+i*N,N,N,LDA,B+i*N,WORK+i*N) ;
    }

    /* unnecessary may be */
    magma_dlacpy(N, NRHS, B , LDB, WORK, N);
    if( NRHS == 1 )
        magmablas_dgemv_MLU(N,N, A,LDA,X,WORK);
    else
        cublasDgemm('N', 'N', N, NRHS, N, alpha, A, LDA, X, LDX, beta, WORK, N);

    /*  Check whether the NRHS normwise backward errors satisfy the
	stopping criterion. If yes, set ITER=IITER>0 and return.     */
    for(i=0;i<NRHS;i++){
      int j,inc=1 ;
      j = cublasIdamax( N , X+i*N  , 1) ;
      cublasGetMatrix( 1, 1, sizeof(double), X+i*N+j-1, 1, XNRM, 1 ) ;
      XNRM[0] =  fabs (XNRM[0]) ;
      j = cublasIdamax ( N ,WORK+i*N , 1 ) ;
      cublasGetMatrix( 1, 1, sizeof(double), WORK+i*N+j-1, 1, RNRM, 1 ) ;
      RNRM[0] =  fabs (RNRM[0]);
      if( RNRM[0] > (XNRM[0]*CTE) ){
        goto L20;
      }
    }

    /*  If we are here, the NRHS normwise backward errors satisfy
        the stopping criterion, we are good to exit.                    */
    *ITER = IITER ;
    return 0;
  L20:
    IITER++ ;
  }

  /* If we are at this place of the code, this is because we have
     performed ITER=ITERMAX iterations and never satisified the
     stopping criterion, set up the ITER flag accordingly and follow
     up on double precision routine.                                    */
  *ITER = -ITERMAX - 1 ;

 L40:
  /* Single-precision iterative refinement failed to converge to a
     satisfactory solution, so we resort to double precision.           */
  magma_dgeqrf_gpu2(M, N, A, N, tau_d, d_work_d, INFO);
  if( *INFO != 0 ){
    return 0;
  }
  magma_dlacpy(N, NRHS, B , LDB, X, N);
  magma_dgeqrs_gpu(M, N, NRHS, A, N, tau_d,
		   X, M, h_work_d, &lwork_d, d_work_d, INFO);
  return 0;
}

#undef max
