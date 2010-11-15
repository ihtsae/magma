/*
    -- MAGMA (version 1.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2010

       @precisions mixed zc -> ds

*/

#include <stdio.h>
#include <math.h>
#include "cuda.h"
#include "cublas.h"
#include "magma.h"
#include "magmablas.h"
#include <cuda_runtime_api.h>

#define BWDMAX 1.0
#define ITERMAX 30

extern "C" magma_int_t
magma_zcgesv_gpu(magma_int_t N, magma_int_t NRHS, double2 *A, magma_int_t LDA, magma_int_t *IPIV, double2 *B, 
		 magma_int_t LDB, double2 *X, magma_int_t LDX, double2 *WORK, float2 *SWORK,
		 magma_int_t *ITER, magma_int_t *INFO, float2 *H_SWORK, double2 *H_WORK,
		 magma_int_t *DIPIV)
{
/*  -- MAGMA (version 1.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2010

    Purpose
    =======

    ZCGESV computes the solution to a real system of linear equations
       A * X = B,
    where A is an N-by-N matrix and X and B are N-by-NRHS matrices.

    ZCGESV first attempts to factorize the matrix in SINGLE PRECISION
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

    N       (input) INTEGER
            The number of linear equations, i.e., the order of the
            matrix A.  N >= 0.

    NRHS    (input) INTEGER
            The number of right hand sides, i.e., the number of columns
            of the matrix B.  NRHS >= 0.

    A       (input or input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the N-by-N coefficient matrix A.
            On exit, if iterative refinement has been successfully used
            (INFO.EQ.0 and ITER.GE.0, see description below), A is
            unchanged. If double2 precision factorization has been used
            (INFO.EQ.0 and ITER.LT.0, see description below), then the
            array A contains the factors L and U from the factorization
            A = P*L*U; the unit diagonal elements of L are not stored.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    IPIV    (output) INTEGER array, dimension (N)
            The pivot indices that define the permutation matrix P;
            row i of the matrix was interchanged with row IPIV(i).
            Corresponzc either to the single precision factorization
            (if INFO.EQ.0 and ITER.GE.0) or the double2 precision
            factorization (if INFO.EQ.0 and ITER.LT.0).

    B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS)
            The N-by-NRHS right hand side matrix B.

    LDB     (input) INTEGER
            The leading dimension of the array B.  LDB >= max(1,N).

    X       (output) DOUBLE PRECISION array, dimension (LDX,NRHS)
            If INFO = 0, the N-by-NRHS solution matrix X.

    LDX     (input) INTEGER
            The leading dimension of the array X.  LDX >= max(1,N).

    WORK    (workspace) DOUBLE PRECISION array, dimension (N*NRHS)
            This array is used to hold the residual vectors.

    SWORK   (workspace) REAL array, dimension (N*(N+NRHS))
            This array is used to store the single precision matrix and the
            right-hand sides or solutions in single precision.

    ITER    (output) INTEGER
            < 0: iterative refinement has failed, double2 precision
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
            > 0:  if INFO = i, U(i,i) computed in DOUBLE PRECISION is
                  exactly zero.  The factorization has been completed,
                  but the factor U is exactly singular, so the solution
                  could not be computed.

    H_SWORK (workspace) REAL array, dimension at least (nb, nb)
            where nb can be obtained through magma_get_sgetrf_nb(*n)
            Work array allocated with cudaMallocHost.

    H_WORK  (workspace) DOUBLE array, dimension at least (nb, nb)
            where nb can be obtained through magma_get_dgetrf_nb(*n)
            Work array allocated with cudaMallocHost.

    DIPIV   (output) INTEGER array on the GPU, dimension (min(M,N))
            The pivot indices; for 1 <= i <= min(M,N), row i of the
            matrix was moved to row IPIV(i).

    =====================================================================    */

  #define max(a,b)       (((a)>(b))?(a):(b))

  double2 c_neg_one = MAGMA_Z_NEG_ONE;
  double2 c_one = MAGMA_Z_ONE;

  int c_ione = 1;

  /*
    Check The Parameters. 
  */
  *ITER = 0 ;
  if ( N <0)
    *INFO = -1;
  else if(NRHS<0)
    *INFO =-2;
  else if(LDA < max(1,N))
    *INFO =-4;
  else if( LDB < max(1,N))
    *INFO =-7;
  else if( LDX < max(1,N))
    *INFO =-9;

  if(*INFO!=0){
    magma_xerbla("magma_zcgesv",INFO) ;
  }

  if( N == 0 || NRHS == 0 )
    return 0;

  double ANRM , CTE, EPS;
  EPS  = lapackf77_dlamch("Epsilon");
  ANRM = magma_zlange('I', N, N , A, LDA , WORK );
  CTE = ANRM * EPS *  pow((double)N,0.5) * BWDMAX ;

  int PTSA  = N*NRHS;
  float RMAX = lapackf77_slamch("O");
  int IITER ;
  double2 alpha = c_neg_one;
  double2 beta = c_one;
  
  float2 RMAX_cplx;
  MAGMA_Z_SET2REAL( RMAX_cplx, RMAX );

  magmablas_zlag2c(N , NRHS , B , LDB , SWORK, N , RMAX_cplx );
  if(*INFO !=0){
    *ITER = -2 ;
    printf("magmablas_zlag2c\n");
    goto L40;
  }
  magmablas_zlag2c(N , N , A , LDA , SWORK+PTSA, LDA , RMAX_cplx); // Merge with DLANGE /
  if(*INFO !=0){
    *ITER = -2 ;
    printf("magmablas_zlag2c\n");
    goto L40;
  }

  double2 XNRM[1] , RNRM[1] ;
  
  magma_cgetrf_gpu(N, N, SWORK+PTSA, LDA, IPIV, INFO);
  
  // Generate parallel pivots
  {
  int *newipiv  = (int*)malloc(N * sizeof(int));
  swp2pswp(N, IPIV, newipiv);
  cudaMemcpy(DIPIV, IPIV, N*sizeof(int), cudaMemcpyHostToDevice);
  free(newipiv);
  }

  if(INFO[0] !=0){
    *ITER = -3 ;
    goto L40;
  }
  magma_zcgetrs_gpu(N, NRHS, SWORK+PTSA, LDA, DIPIV, SWORK, B, LDB, INFO);
  int i,j ;
  magmablas_clag2z(N , NRHS , SWORK, N , X , LDX , INFO );
  magma_zlacpy(N, NRHS, B , LDB, WORK, N);
  if ( NRHS == 1 )
     magmablas_zgemv_MLU(N, N, A, LDA, X, WORK);
  else
     cublasZgemm( 'N', 'N', N, NRHS, N, c_neg_one, A, LDA, X, LDX, c_one, WORK, N);


  for(i=0;i<NRHS;i++){
    j = cublasIzamax( N ,X+i*N, 1) ;
    cublasGetMatrix( 1, 1, sizeof(double2), X+i*N+j-1, 1, XNRM, 1);
    MAGMA_Z_SET2REAL( XNRM[0], lapackf77_zlange( "F", &c_ione, &c_ione, XNRM, &c_ione, NULL ) );
    j = cublasIzamax ( N, WORK+i*N, 1 );
    cublasGetMatrix( 1, 1, sizeof(double2), WORK+i*N+j-1, 1, RNRM, 1 );
    MAGMA_Z_SET2REAL( RNRM[0], lapackf77_zlange( "F", &c_ione, &c_ione, RNRM, &c_ione, NULL ) );
    // printf("\n\t\t--   %lf  %lf --\n", RNRM[0] , XNRM[0]*CTE ); 
    if( MAGMA_Z_GET_X( RNRM[0] ) > MAGMA_Z_GET_X( XNRM[0] ) *CTE ){
      goto L10;
    }
  }
 
  *ITER =0;
  return 0;
 L10:
  ;

  for(IITER=1;IITER<ITERMAX;)
    {
      *INFO = 0 ;
      /*
        Convert R (in WORK) from double2 precision to single precision
        and store the result in SX.
        Solve the system SA*SX = SR.
        -- These two Tasks are merged here. 
      */
      magma_zcgetrs_gpu( N, NRHS, SWORK+PTSA, LDA, DIPIV, SWORK, WORK, LDB, INFO);
      if(INFO[0] !=0){
	*ITER = -3 ;
	goto L40;
      }
      for(i=0;i<NRHS;i++){
	magmablas_zcaxpycp(SWORK+i*N,X+i*N,N,N,LDA,B+i*N,WORK+i*N) ;
      }
      
      magma_zlacpy(N, NRHS, B , LDB, WORK, N);
      if( NRHS == 1 )
        magmablas_zgemv_MLU(N, N, A, LDA, X, WORK);
      else
        cublasZgemm( 'N', 'N', N, NRHS, N, alpha, A, LDA, X, LDX, beta, WORK, N);

      /*
        Check whether the NRHS normwise backward errors satisfy the
        stopping criterion. If yes, set ITER=IITER>0 and return.
      */
      for(i=0;i<NRHS;i++){
	int j;
	j = cublasIzamax( N , X+i*N  , 1) ;
	cublasGetMatrix( 1, 1, sizeof(double2), X+i*N+j-1, 1, XNRM, 1 ) ;
	MAGMA_Z_SET2REAL( XNRM[0],  lapackf77_zlange( "F", &c_ione, &c_ione, XNRM, &c_ione, NULL ) );
	j = cublasIzamax ( N ,WORK+i*N , 1 ) ;
	cublasGetMatrix( 1, 1, sizeof(double2), WORK+i*N+j-1, 1, RNRM, 1 ) ;
	MAGMA_Z_SET2REAL( RNRM[0],  lapackf77_zlange( "F", &c_ione, &c_ione, RNRM, &c_ione, NULL ) );
	if( MAGMA_Z_GET_X( RNRM[0] ) > (MAGMA_Z_GET_X( XNRM[0] )*CTE) ){
	  goto L20;
	}
      }
      /*
        If we are here, the NRHS normwise backward errors satisfy the
        stopping criterion, we are good to exit.
      */
      
      *ITER = IITER ;
      return 0;
    L20:
      IITER++ ;
    }
  /*
    If we are at this place of the code, this is because we have
    performed ITER=ITERMAX iterations and never satisified the
    stopping criterion, set up the ITER flag accordingly and follow up
    on double2 precision routine.
  */
  *ITER = -ITERMAX - 1 ;
  
 L40:
  /*
    Single-precision iterative refinement failed to converge to a
    satisfactory solution, so we resort to double2 precision.  
  */
  if( *INFO != 0 ){
    return 0;
  }

  printf("uncomment when double2 precision is available\n"); 
  /*
    magma_zgetrf_gpu(N, N, A, LDA, IPIV, INFO);
    magma_zlacpy(N, NRHS, B , LDB, X, N);
    magma_zgetrs_gpu('N', N, NRHS, A, LDA, IPIV, X, N, INFO, H_WORK);
  */
  return 0;
}

#undef max
