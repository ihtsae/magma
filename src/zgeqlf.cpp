/*
    -- MAGMA (version 1.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2010

       @precisions normal z -> s d c

*/

#include <stdio.h>
#include <cuda_runtime_api.h>
#include <cublas.h>
#include "magma.h"

extern "C" magma_int_t
magma_zgeqlf(magma_int_t m_, magma_int_t n_, double2 *a, magma_int_t lda_, 
	     double2 *tau, double2 *work, magma_int_t *lwork, magma_int_t *info)
{
/*  -- MAGMA (version 1.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2010

    Purpose   
    =======   

    SGEQLF computes a QL factorization of a real M-by-N matrix A:   
    A = Q * L.   

    Arguments   
    =========   

    M       (input) INTEGER   
            The number of rows of the matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix A.  N >= 0.   

    A       (input/output) COMPLEX_16 array, dimension (LDA,N)   
            On entry, the M-by-N matrix A.   
            On exit, if m >= n, the lower triangle of the subarray   
            A(m-n+1:m,1:n) contains the N-by-N lower triangular matrix L;   
            if m <= n, the elements on and below the (n-m)-th   
            superdiagonal contain the M-by-N lower trapezoidal matrix L;   
            the remaining elements, with the array TAU, represent the   
            orthogonal matrix Q as a product of elementary reflectors   
            (see Further Details).   

            Higher performance is achieved if A is in pinned memory, e.g.
            allocated using cudaMallocHost.

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    TAU     (output) COMPLEX_16 array, dimension (min(M,N))   
            The scalar factors of the elementary reflectors (see Further   
            Details).   

    WORK    (workspace/output) COMPLEX_16 array, dimension (MAX(1,LWORK))   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.   

            Higher performance is achieved if WORK is in pinned memory, e.g.
            allocated using cudaMallocHost.

    LWORK   (input) INTEGER   
            The dimension of the array WORK.  LWORK >= max(1,N).   
            For optimum performance LWORK >= N*NB, where NB can be obtained
            through magma_get_zgeqlf_nb(M). 

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit   
            < 0:  if INFO = -i, the i-th argument had an illegal value   
                  if INFO = -9, the GPU memory allocation failed

    Further Details   
    ===============   

    The matrix Q is represented as a product of elementary reflectors   

       Q = H(k) . . . H(2) H(1), where k = min(m,n).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(m-k+i+1:m) = 0 and v(m-k+i) = 1; v(1:m-k+i-1) is stored on exit in   
    A(1:m-k+i-1,n-k+i), and tau in TAU(i).   

    =====================================================================    */

    #define  a_ref(a_1,a_2) ( a+(a_2)*(*lda) + (a_1))
    #define da_ref(a_1,a_2) (da+(a_2)*ldda   + (a_1))
    #define min(a,b)  (((a)<(b))?(a):(b))
    #define max(a,b)  (((a)>(b))?(a):(b))
    
    int *m = &m_;
    int *n = &n_;
    int *lda = &lda_;

    int i, k, lddwork, old_i, old_ib;  

    int rows, cols;
    static int ib, ki, kk, mu, nu, nx, nbmin, iinfo, ldda;
    long int lquery;

    *info = 0;
    int nb = magma_get_zgeqlf_nb(*m);

    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < max(1,*m)) {
	*info = -4;
    }

    if (*info == 0) {
	k = min(*m,*n);
	if (k == 0)
	  work[0] = (double2)1;
	else 
	  work[0] = (double2)*n * nb;

	if (*lwork < max(1,*n) && ! lquery)
	  *info = -7;
    }
    
    if (*info != 0)
      return 0;
    else if (lquery)
      return 0;

    /* Quick return if possible */
    if (k == 0)
      return 0;

    cublasStatus status;
    static cudaStream_t stream[2];
    cudaStreamCreate(&stream[0]);
    cudaStreamCreate(&stream[1]);

    nbmin = 2;
    nx = nb;

    lddwork = ((*n+31)/32)*32;
    ldda    = ((*m+31)/32)*32;

    double2 *da;
    status = cublasAlloc((*n)*ldda + nb*lddwork, sizeof(double2), (void**)&da);
    if (status != CUBLAS_STATUS_SUCCESS) {
      *info = -9;
      return 0;
    }
    double2 *dwork = da + ldda*(*n);

    if (nb >= nbmin && nb < k && nx < k) {
        /*  Use blocked code initially.   
	    The last kk columns are handled by the block method.
            First, copy the matrix on the GPU except the last kk columns */
        cudaMemcpy2DAsync(da_ref(0, 0),  ldda *sizeof(double2),
			  a_ref(0, 0), (*lda)*sizeof(double2),
			  sizeof(double2)*(*m), (*n-nb),
			  cudaMemcpyHostToDevice,stream[0]);

        ki = ((k - nx - 1) / nb) * nb;
	kk = min(k, ki + nb);
	for (i = k - kk + ki; i >= k -kk; i -= nb) {
	    ib = min(k-i,nb);
 
	    if (i< k - kk + ki){
	      /* 1. Copy asynchronously the current panel to the CPU.
		 2. Copy asynchronously the submatrix below the panel 
		    to the CPU)                                        */
	      rows = *m - k + i + ib;
	      cudaMemcpy2DAsync(  a_ref(0,*n-k+i), (*lda)*sizeof(double2),
				  da_ref(0,*n-k+i), ldda *sizeof(double2),
				  sizeof(double2)*rows, ib,
				  cudaMemcpyDeviceToHost,stream[1]);

	      cudaMemcpy2DAsync(  a_ref(rows,*n-k+i), (*lda)*sizeof(double2),
				  da_ref(rows,*n-k+i), ldda *sizeof(double2),
				  sizeof(double2)*(*m-rows), ib,
				  cudaMemcpyDeviceToHost,stream[0]);

	      /* Apply H' to A(1:m-k+i+ib-1,1:n-k+i-1) from the left in
		 two steps - implementing the lookahead techniques.
		 This is the main update from the lookahead techniques. */
	      rows = *m - k + old_i + old_ib;
              cols = *n - k + old_i - old_ib;
              magma_zlarfb('B', 'C', rows, cols, old_ib,
                           da_ref(0,cols+old_ib), ldda, dwork, lddwork,
                           da_ref(0, 0), ldda, dwork+old_ib, lddwork);
	    }

	    cudaStreamSynchronize(stream[1]);
	    /* Compute the QL factorization of the current block   
	       A(1:m-k+i+ib-1,n-k+i:n-k+i+ib-1) */
	    rows = *m - k + i + ib;
	    cols = *n - k + i;
	    zgeqlf_(&rows,&ib, a_ref(0,cols), lda, tau+i, work,lwork,&iinfo);

	    if (cols > 0) {
	        /* Form the triangular factor of the block reflector   
		   H = H(i+ib-1) . . . H(i+1) H(i) */
	        zlarft_("B", "C", &rows, &ib, a_ref(0, cols), lda, 
			tau + i, work, &ib);
       
		zpanel_to_q('L', ib, a_ref(rows-ib,cols), *lda, work+ib*ib);
		cublasSetMatrix(rows, ib, sizeof(double2),
				a_ref(0,cols), *lda, da_ref(0,cols), ldda);
		zq_to_panel('L', ib, a_ref(rows-ib,cols), *lda, work+ib*ib);

		// Send the triangular part on the GPU
		cublasSetMatrix(ib,ib,sizeof(double2), work, ib, dwork, lddwork);

		/* Apply H' to A(1:m-k+i+ib-1,1:n-k+i-1) from the left in
		   two steps - implementing the lookahead techniques.
		   This is the update of first ib columns.                 */
		if (i-ib >= k -kk)
		  magma_zlarfb('B', 'C', rows, ib, ib,
			       da_ref(0,cols), ldda, dwork, lddwork,
			       da_ref(0,cols-ib), ldda, dwork+ib, lddwork);
		else{
		  magma_zlarfb('B', 'C', rows, cols, ib,
                               da_ref(0,cols), ldda, dwork, lddwork,
                               da_ref(0,0), ldda, dwork+ib, lddwork);
		}

		old_i = i;
		old_ib = ib;
	    }
	}
	mu = *m - k + i + nb;
	nu = *n - k + i + nb;
    } else {
	mu = *m;
	nu = *n;
    }

    /* Use unblocked code to factor the last or only block */
    if (mu > 0 && nu > 0){
      cublasGetMatrix(*m, nu, sizeof(double2),
		      da_ref(0,0), ldda, a_ref(0,0), *lda);

      zgeqlf_(&mu, &nu, a_ref(0,0), lda, tau, work, lwork, &iinfo);
    }

    cublasFree(da);
    return 0;
    
    /* End of MAGMA_ZGEQLF */

} /* magma_zgeqlf */

#undef  a_ref
#undef da_ref
#undef min
#undef max
