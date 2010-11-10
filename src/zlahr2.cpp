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
#include "magmablas.h"

extern "C" magma_int_t 
magma_zlahr2(magma_int_t n_, magma_int_t k_, magma_int_t nb_, 
	     double2 *da, double2 *dv, double2 *a, magma_int_t lda_, 
	     double2 *tau, double2 *t, magma_int_t ldt_, double2 *y, magma_int_t ldy_)
{
/*  -- MAGMA auxiliary routine (version 1.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2010

    Purpose   
    =======   

    SLAHR2 reduces the first NB columns of a real general n-BY-(n-k+1)   
    matrix A so that elements below the k-th subdiagonal are zero. The   
    reduction is performed by an orthogonal similarity transformation   
    Q' * A * Q. The routine returns the matrices V and T which determine   
    Q as a block reflector I - V*T*V', and also the matrix Y = A * V.   

    This is an auxiliary routine called by SGEHRD.   

    Arguments   
    =========   

    N       (input) INTEGER   
            The order of the matrix A.   

    K       (input) INTEGER   
            The offset for the reduction. Elements below the k-th   
            subdiagonal in the first NB columns are reduced to zero.   
            K < N.   

    NB      (input) INTEGER   
            The number of columns to be reduced.

    DA      (input/output) COMPLEX_16 array on the GPU, dimension (LDA,N-K+1)   
            On entry, the n-by-(n-k+1) general matrix A.   
            On exit, the elements on and above the k-th subdiagonal in   
            the first NB columns are overwritten with the corresponding   
            elements of the reduced matrix; the elements below the k-th   
            subdiagonal, with the array TAU, represent the matrix Q as a   
            product of elementary reflectors. The other columns of A are   
            unchanged. See Further Details.   

    DV      

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,N).   

    TAU     (output) DOUBLE PRECISION array, dimension (NB)   
            The scalar factors of the elementary reflectors. See Further   
            Details.   

    T       (output) DOUBLE PRECISION array, dimension (LDT,NB)   
            The upper triangular matrix T.   

    LDT     (input) INTEGER   
            The leading dimension of the array T.  LDT >= NB.   

    Y       (output) DOUBLE PRECISION array, dimension (LDY,NB)   
            The n-by-nb matrix Y.   

    LDY     (input) INTEGER   
            The leading dimension of the array Y. LDY >= N.   

    Further Details   
    ===============   

    The matrix Q is represented as a product of nb elementary reflectors   

       Q = H(1) H(2) . . . H(nb).   

    Each H(i) has the form   

       H(i) = I - tau * v * v'   

    where tau is a real scalar, and v is a real vector with   
    v(1:i+k-1) = 0, v(i+k) = 1; v(i+k+1:n) is stored on exit in   
    A(i+k+1:n,i), and tau in TAU(i).   

    The elements of the vectors v together form the (n-k+1)-by-nb matrix   
    V which is needed, with T and Y, to apply the transformation to the   
    unreduced part of the matrix, using an update of the form:   
    A := (I - V*T*V') * (A - Y*T*V').   

    The contents of A on exit are illustrated by the following example   
    with n = 7, k = 3 and nb = 2:   

       ( a   a   a   a   a )   
       ( a   a   a   a   a )   
       ( a   a   a   a   a )   
       ( h   h   a   a   a )   
       ( v1  h   a   a   a )   
       ( v1  v2  a   a   a )   
       ( v1  v2  a   a   a )   

    where a denotes an element of the original matrix A, h denotes a   
    modified element of the upper Hessenberg matrix H, and vi denotes an   
    element of the vector defining H(i).

    This implementation follows the hybrid algorithm and notations described in

    S. Tomov and J. Dongarra, "Accelerating the reduction to upper Hessenberg
    form through hybrid GPU-based computing," University of Tennessee Computer
    Science Technical Report, UT-CS-09-642 (also LAPACK Working Note 219),
    May 24, 2009.

    =====================================================================    */

    #define min(a,b) ((a) <= (b) ? (a) : (b))

    double2 c_zero = MAGMA_Z_ONE;
    double2 c_one = MAGMA_Z_ONE;
    double2 c_neg_one = MAGMA_Z_NEG_ONE;

    int *n = &n_;
    int *nb = &nb_;
    int *k = &k_;
    int *lda = &lda_;
    int *ldt = &ldt_;
    int *ldy = &ldy_;

    int ldda = *n;

    /* Table of constant values */
    static int c__1 = 1;
    
    /* System generated locals */
    int a_dim1, a_offset, t_dim1, t_offset, y_dim1, y_offset, i__2, i__3;
    double2 d__1;
    /* Local variables */
    static int i__;
    static double2 ei;

    --tau;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    y_dim1 = *ldy;
    y_offset = 1 + y_dim1;
    y -= y_offset;

    /* Function Body */
    if (*n <= 1)
      return 0;
    

    for (i__ = 1; i__ <= *nb; ++i__) {
	if (i__ > 1) {

	  /* Update A(K+1:N,I); Update I-th column of A - Y * V' */
	  i__2 = *n - *k + 1;
	  i__3 = i__ - 1;
	  blasf77_zcopy(&i__3, &a[*k+i__-1+a_dim1], lda, &t[*nb*t_dim1+1], &c__1);
	  blasf77_ztrmv("u","n","n",&i__3,&t[t_offset], ldt, &t[*nb*t_dim1+1], &c__1);
	  blasf77_zgemv("NO TRANSPOSE", &i__2, &i__3, &c_neg_one, &y[*k + y_dim1],
		 ldy, &t[*nb*t_dim1+1], &c__1, &c_one, &a[*k+i__*a_dim1],&c__1);

	  /* Apply I - V * T' * V' to this column (call it b) from the   
             left, using the last column of T as workspace   

             Let  V = ( V1 )   and   b = ( b1 )   (first I-1 rows)   
                      ( V2 )             ( b2 )   
             where V1 is unit lower triangular   
             w := V1' * b1                                                 */
	  
	  i__2 = i__ - 1;
	  blasf77_zcopy(&i__2, &a[*k+1+i__*a_dim1], &c__1, &t[*nb*t_dim1+1], &c__1);
	  blasf77_ztrmv("Lower", "Transpose", "UNIT", &i__2, &a[*k + 1 + a_dim1], 
		 lda, &t[*nb * t_dim1 + 1], &c__1);

	  /* w := w + V2'*b2 */
	  i__2 = *n - *k - i__ + 1;
	  i__3 = i__ - 1;
	  blasf77_zgemv("T", &i__2, &i__3, &c_one, &a[*k + i__ + a_dim1], lda, 
		 &a[*k+i__+i__*a_dim1], &c__1, &c_one, &t[*nb*t_dim1+1], &c__1);

	  /* w := T'*w */
	  i__2 = i__ - 1;
	  blasf77_ztrmv("U","T","N",&i__2, &t[t_offset], ldt, &t[*nb*t_dim1+1],&c__1);
	  
	  /* b2 := b2 - V2*w */
	  i__2 = *n - *k - i__ + 1;
	  i__3 = i__ - 1;
	  blasf77_zgemv("N", &i__2, &i__3, &c_neg_one, &a[*k + i__ + a_dim1], lda, 
		 &t[*nb*t_dim1+1], &c__1, &c_one, &a[*k+i__+i__*a_dim1], &c__1);

	  /* b1 := b1 - V1*w */
	  i__2 = i__ - 1;
	  blasf77_ztrmv("L","N","U",&i__2,&a[*k+1+a_dim1],lda,&t[*nb*t_dim1+1],&c__1);
	  blasf77_zaxpy(&i__2, &c_neg_one, &t[*nb * t_dim1 + 1], &c__1, 
		 &a[*k + 1 + i__ * a_dim1], &c__1);
	  
	  a[*k + i__ - 1 + (i__ - 1) * a_dim1] = ei;
	}
	
	/* Generate the elementary reflector H(I) to annihilate A(K+I+1:N,I) */
	i__2 = *n - *k - i__ + 1;
	i__3 = *k + i__ + 1;
	lapackf77_zlarfg(&i__2, &a[*k + i__ + i__ * a_dim1], 
                         &a[min(i__3,*n) + i__ * a_dim1], &c__1, &tau[i__]);
	ei = a[*k + i__ + i__ * a_dim1];
	a[*k + i__ + i__ * a_dim1] = c_one;

	/* Compute  Y(K+1:N,I) */
        i__2 = *n - *k;
	i__3 = *n - *k - i__ + 1;
        cublasSetVector(i__3, sizeof(double2), 
                        &a[*k + i__ + i__*a_dim1], 1, dv+(i__-1)*(ldda+1), 1);

	cublasZgemv('N', i__2+1, i__3, c_one, 
		    da -1 + *k + i__ *ldda, ldda, 
		    dv+(i__-1)*(ldda+1), c__1, c_zero, 
		    da-1 + *k + (i__-1)*ldda, c__1);     
	
	i__2 = *n - *k - i__ + 1;
	i__3 = i__ - 1;
	blasf77_zgemv("T", &i__2, &i__3, &c_one, &a[*k + i__ + a_dim1], lda,
	       &a[*k+i__+i__*a_dim1], &c__1, &c_zero, &t[i__*t_dim1+1], &c__1);

	/* Compute T(1:I,I) */
	i__2 = i__ - 1;
	MAGMA_Z_OP_NEG_ASGN( d__1, tau[i__] );
	blasf77_zscal(&i__2, &d__1, &t[i__ * t_dim1 + 1], &c__1);
	blasf77_ztrmv("U","N","N", &i__2, &t[t_offset], ldt, &t[i__*t_dim1+1], &c__1);
	t[i__ + i__ * t_dim1] = tau[i__];

        cublasGetVector(*n - *k + 1, sizeof(double2),
	                da-1+ *k+(i__-1)*ldda, 1, y+ *k + i__*y_dim1, 1);
    }
    a[*k + *nb + *nb * a_dim1] = ei;

    return 0;

    /* End of MAGMA_ZLAHR2 */

} /* magma_zlahr2 */

#undef min
