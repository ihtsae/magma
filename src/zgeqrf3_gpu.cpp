/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2011

       @precisions normal z -> s d c

*/
#include "common_magma.h"

/* ////////////////////////////////////////////////////////////////////////////
   -- Auxiliary function: 'a' is pointer to the current panel holding the 
      Householder vectors for the QR factorization of the panel. This routine
      puts ones on the diagonal and zeros in the upper triangular part of 'a'.
      The upper triangular values are stored in work. 
 */
void zsplit_diag_block3(int ib, magmaDoubleComplex *a, int lda, magmaDoubleComplex *work){
    int i, j;
    magmaDoubleComplex *cola, *colw;
    magmaDoubleComplex c_zero = MAGMA_Z_ZERO;
    magmaDoubleComplex c_one  = MAGMA_Z_ONE;

    for(i=0; i<ib; i++){
        cola = a    + i*lda;
        colw = work + i*ib;
        for(j=0; j<i; j++){
            colw[j] = cola[j];
            cola[j] = c_zero;
        }
        colw[i] = cola[i];
        cola[i] = c_one;
    }
}

extern "C" magma_int_t
magma_zgeqrf3_gpu( magma_int_t m, magma_int_t n, 
                  magmaDoubleComplex *dA,   magma_int_t ldda,
                  magmaDoubleComplex *tau, magmaDoubleComplex *dT, 
                  magma_int_t *info )
{
/*  -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2011

    Purpose
    =======
    ZGEQRF3 computes a QR factorization of a complex M-by-N matrix A:
    A = Q * R.
    
    This version stores the triangular dT matrices used in
    the block QR factorization so that they can be applied directly (i.e.,
    without being recomputed) later. As a result, the application
    of Q is much faster. Also, the upper triangular matrices for V have 0s
    in them and the corresponding parts of the upper triangular R are
    stored separately in dT.

    Arguments
    =========
    M       (input) INTEGER
            The number of rows of the matrix A.  M >= 0.

    N       (input) INTEGER
            The number of columns of the matrix A.  N >= 0.

    dA      (input/output) COMPLEX_16 array on the GPU, dimension (LDDA,N)
            On entry, the M-by-N matrix A.
            On exit, the elements on and above the diagonal of the array
            contain the min(M,N)-by-N upper trapezoidal matrix R (R is
            upper triangular if m >= n); the elements below the diagonal,
            with the array TAU, represent the orthogonal matrix Q as a
            product of min(m,n) elementary reflectors (see Further
            Details).

    LDDA    (input) INTEGER
            The leading dimension of the array dA.  LDDA >= max(1,M).
            To benefit from coalescent memory accesses LDDA must be
            dividable by 16.

    TAU     (output) COMPLEX_16 array, dimension (min(M,N))
            The scalar factors of the elementary reflectors (see Further
            Details).

    dT      (workspace/output)  COMPLEX_16 array on the GPU, 
            dimension (2*MIN(M, N) + (N+31)/32*32 )*NB,
            where NB can be obtained through magma_get_zgeqrf_nb(M).
            It starts with MIN(M,N)*NB block that store the triangular T
            matrices, followed by the MIN(M,N)*NB block of the diagonal
            matrices for the R matrix. The rest of the array is used as workspace.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
                  or another error occured, such as memory allocation failed.

    Further Details
    ===============
    The matrix Q is represented as a product of elementary reflectors

       Q = H(1) H(2) . . . H(k), where k = min(m,n).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a complex scalar, and v is a complex vector with
    v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in A(i+1:m,i),
    and tau in TAU(i).
    =====================================================================    */

    #define a_ref(a_1,a_2) (dA+(a_2)*(ldda) + (a_1))
    #define t_ref(a_1)     (dT+(a_1)*nb)
    #define d_ref(a_1)     (dT+(minmn+(a_1))*nb)
    #define dd_ref(a_1)    (dT+(2*minmn+(a_1))*nb)
    #define work_ref(a_1)  ( work + (a_1))
    #define hwork          ( work + (nb)*(m))

    magma_int_t i, k, minmn, old_i, old_ib, rows, cols;
    magma_int_t ib, nb;
    magma_int_t ldwork, lddwork, lwork, lhwork;
    magmaDoubleComplex *work, *ut;

    /* check arguments */
    *info = 0;
    if (m < 0) {
        *info = -1;
    } else if (n < 0) {
        *info = -2;
    } else if (ldda < max(1,m)) {
        *info = -4;
    }
    if (*info != 0) {
        magma_xerbla( __func__, -(*info) );
        return *info;
    }

    k = minmn = min(m,n);
    if (k == 0)
        return *info;

    nb = magma_get_zgeqrf_nb(m);

    lwork  = (m + n + nb)*nb;
    lhwork = lwork - m*nb;

    if (MAGMA_SUCCESS != magma_zmalloc_pinned( &work, lwork )) {
        *info = MAGMA_ERR_HOST_ALLOC;
        return *info;
    }
    
    ut = hwork+nb*(n);
    memset( ut, 0, nb*nb*sizeof(magmaDoubleComplex));

    magma_queue_t stream[2];
    magma_queue_create( &stream[0] );
    magma_queue_create( &stream[1] );

    ldwork = m;
    lddwork= n;

    if ( (nb > 1) && (nb < k) ) {
        /* Use blocked code initially */
        old_i = 0; old_ib = nb;
        for (i = 0; i < k-nb; i += nb) {
            ib = min(k-i, nb);
            rows = m -i;
            magma_zgetmatrix_async( rows, ib,
                                    a_ref(i,i),  ldda,
                                    work_ref(i), ldwork, stream[1] );
            if (i>0){
                /* Apply H' to A(i:m,i+2*ib:n) from the left */
                cols = n-old_i-2*old_ib;
                magma_zlarfb_gpu( MagmaLeft, MagmaConjTrans, MagmaForward, MagmaColumnwise,
                                  m-old_i, cols, old_ib,
                                  a_ref(old_i, old_i         ), ldda, t_ref(old_i), nb,
                                  a_ref(old_i, old_i+2*old_ib), ldda, dd_ref(0),    lddwork);
                
                /* store the diagonal */
                magma_zsetmatrix_async( old_ib, old_ib,
                                        ut,           old_ib,
                                        d_ref(old_i), old_ib, stream[0] );
            }

            magma_queue_sync( stream[1] );
            lapackf77_zgeqrf(&rows, &ib, work_ref(i), &ldwork, tau+i, hwork, &lhwork, info);
            /* Form the triangular factor of the block reflector
               H = H(i) H(i+1) . . . H(i+ib-1) */
            lapackf77_zlarft( MagmaForwardStr, MagmaColumnwiseStr, 
                              &rows, &ib, 
                              work_ref(i), &ldwork, tau+i, hwork, &ib);

            /* Put 0s in the upper triangular part of a panel (and 1s on the
               diagonal); copy the upper triangular in ut.     */
            magma_queue_sync( stream[0] );
            zsplit_diag_block3(ib, work_ref(i), ldwork, ut);
            magma_zsetmatrix( rows, ib, work_ref(i), ldwork, a_ref(i,i), ldda );

            if (i + ib < n) {
                /* Send the triangular factor T to the GPU */
                magma_zsetmatrix( ib, ib, hwork, ib, t_ref(i), nb );

                if (i+nb < k-nb){
                    /* Apply H' to A(i:m,i+ib:i+2*ib) from the left */
                    magma_zlarfb_gpu( MagmaLeft, MagmaConjTrans, MagmaForward, MagmaColumnwise,
                                      rows, ib, ib, 
                                      a_ref(i, i   ), ldda, t_ref(i),  nb, 
                                      a_ref(i, i+ib), ldda, dd_ref(0), lddwork);
                }
                else {
                    cols = n-i-ib;
                    magma_zlarfb_gpu( MagmaLeft, MagmaConjTrans, MagmaForward, MagmaColumnwise,
                                      rows, cols, ib, 
                                      a_ref(i, i   ), ldda, t_ref(i),  nb, 
                                      a_ref(i, i+ib), ldda, dd_ref(0), lddwork);
                    /* Fix the diagonal block */
                    magma_zsetmatrix( ib, ib, ut, ib, d_ref(i), ib );
                }
                old_i  = i;
                old_ib = ib;
            }
        }
    } else {
        i = 0;
    }

    /* Use unblocked code to factor the last or only block. */
    if (i < k) {
        ib   = n-i;
        rows = m-i;
        magma_zgetmatrix( rows, ib, a_ref(i, i), ldda, work, rows );
        lhwork = lwork - rows*ib;
        lapackf77_zgeqrf(&rows, &ib, work, &rows, tau+i, work+ib*rows, &lhwork, info);
        
        magma_zsetmatrix( rows, ib, work, rows, a_ref(i, i), ldda );
    }

    magma_queue_destroy( stream[0] );
    magma_queue_destroy( stream[1] );
    magma_free_pinned( work );
    return *info;

/*     End of MAGMA_ZGEQRF */

} /* magma_zgeqrf */

#undef a_ref
#undef t_ref
#undef d_ref
#undef work_ref
