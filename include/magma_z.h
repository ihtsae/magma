/*
 *   -- MAGMA (version 1.0) --
 *      Univ. of Tennessee, Knoxville
 *      Univ. of California, Berkeley
 *      Univ. of Colorado, Denver
 *      November 2010
 *
 * @precisions normal z -> s d c
 */

#ifndef _MAGMA_Z_H_
#define _MAGMA_Z_H_

#ifdef __cplusplus
extern "C" {
#endif

  /*
   * TODO: 
   *    - remove the info to return the value as output of the fnction and not as parameter
   *    - makes consistent the order to pass host and device pointers
   *    - Correct the names of mixed precision functions sd => ds 
   */

/* ////////////////////////////////////////////////////////////////////////////
   -- MAGMA function definitions / Data on CPU
*/
magma_int_t magma_zgebrd( magma_int_t m, magma_int_t n, double2 *A, magma_int_t lda, double2 *d, double2 *e, double2 *tauq,  double2 *taup, double2 *work, magma_int_t lwork, double2 *da, magma_int_t *info);
magma_int_t magma_zgehrd( magma_int_t n, magma_int_t ilo, magma_int_t ihi, double2 *A, magma_int_t lda, double2 *tau, double2 *work, magma_int_t lwork, double2 *da, magma_int_t *info);
magma_int_t magma_zgelqf( magma_int_t m, magma_int_t n, double2 *A, magma_int_t lda, double2 *tau, double2 *work, magma_int_t lwork, double2 *da, magma_int_t *info);
magma_int_t magma_zgelqf2(magma_int_t m, magma_int_t n, double2 *A, magma_int_t lda, double2 *tau, double2 *work, magma_int_t lwork, double2 *da, magma_int_t *info);
magma_int_t magma_zgeqlf( magma_int_t m, magma_int_t n, double2 *A, magma_int_t lda, double2 *tau, double2 *work, magma_int_t lwork, double2 *da, magma_int_t *info);
magma_int_t magma_zgeqrf( magma_int_t m, magma_int_t n, double2 *A, magma_int_t lda, double2 *tau, double2 *work, magma_int_t lwork, double2 *da, magma_int_t *info);
magma_int_t magma_zgeqrf2(magma_int_t m, magma_int_t n, double2 *A, magma_int_t lda, double2 *tau, double2 *work, magma_int_t lwork,              magma_int_t *info);
magma_int_t magma_zgetrf( magma_int_t m, magma_int_t n, double2 *A, magma_int_t lda, magma_int_t *ipiv, magma_int_t *info);
magma_int_t magma_zlabrd( magma_int_t m, magma_int_t n, magma_int_t nb, 
			  double2 *a,  magma_int_t lda,  double2 *d,  double2 * e, 
			  double2 *tauq, double2 *taup,  double2 *x,  magma_int_t ldx,  
			  double2 *y,  magma_int_t ldy,  double2 *da, magma_int_t ldda, 
			  double2 *dx, magma_int_t lddx, double2 *dy, magma_int_t *lddy);
magma_int_t magma_zlahr2( magma_int_t m, magma_int_t n, magma_int_t nb, 
			  double2 *da, double2 *dv, double2 *a, magma_int_t lda, double2 *tau, 
			  double2 *t, magma_int_t ldt, double2 *y, magma_int_t ldy);
magma_int_t magma_zlahru( magma_int_t m, magma_int_t n, magma_int_t nb, double2 *a, magma_int_t lda, 
			  double2 *da, double2 *y, double2 *v, double2 *t, double2 *dwork);
magma_int_t magma_zlarfb( char direct, char storev, magma_int_t m, magma_int_t n, magma_int_t k, 
			  double2 *dv, magma_int_t ldv, double2 *dt,    magma_int_t ldt, 
			  double2 *dc, magma_int_t ldc, double2 *dowrk, magma_int_t ldwork);
magma_int_t magma_zpotrf( char uplo, magma_int_t n, double2 *A, magma_int_t lda, double2 *work, magma_int_t *info);
magma_int_t magma_zpotrf2(char uplo, magma_int_t n, double2 *A, magma_int_t lda,                magma_int_t *info);
magma_int_t magma_zpotrf3(char uplo, magma_int_t n, double2 *A, magma_int_t lda, double2 *work, magma_int_t *info);
magma_int_t magma_zsytrd( char uplo, magma_int_t n, double2 *A, magma_int_t lda, 
			  double2 *d, double2 *e, double2 *tau, 
			  double2 *work, magma_int_t lwork, double2 *da, magma_int_t *info);

/* //////////////////////////////////////////////////////////////////////////// 
 -- MAGMA function definitions / Data on GPU
*/
magma_int_t magma_zgeqrf_gpu( magma_int_t m, magma_int_t n, double2 *A, magma_int_t lda, double2 *tau, double2 *work, magma_int_t lwork, double2 *dwork, magma_int_t *info);
magma_int_t magma_zgeqrf_gpu2(magma_int_t m, magma_int_t n, double2 *A, magma_int_t lda, double2 *tau, double2 *work, magma_int_t lwork, double2 *dwork, magma_int_t *info);
magma_int_t magma_zgeqrf_gpu3(magma_int_t m, magma_int_t n, double2 *A, magma_int_t lda, double2 *tau, double2 *work, magma_int_t lwork, double2 *dwork, magma_int_t *info);
magma_int_t magma_zgeqrs_gpu( magma_int_t m, magma_int_t n, magma_int_t nrhs, double2 *A, magma_int_t lda, double2 *tau, double2 *c, magma_int_t ldc, double2 *work, magma_int_t lwork, double2 *td, magma_int_t *info);
magma_int_t magma_zgetrf_gpu( magma_int_t m, magma_int_t n, double2 *A, magma_int_t lda, magma_int_t *ipiv, magma_int_t *info);
magma_int_t magma_zgetrf_gpu2(magma_int_t m, magma_int_t n, double2 *A, magma_int_t lda, magma_int_t *ipiv, magma_int_t *dipiv, double2 *work, magma_int_t *info);
magma_int_t magma_zgetrs_gpu( char trans, magma_int_t n, magma_int_t nrhs, double2 *A, magma_int_t lda, magma_int_t *ipiv, double2 *b, magma_int_t ldb, magma_int_t *info, double2 *hwork);
magma_int_t magma_zpotrf_gpu( char uplo,  magma_int_t n, double2 *A, magma_int_t lda, double2 *work, magma_int_t *info);
magma_int_t magma_zpotrs_gpu( char uplo,  magma_int_t n, magma_int_t nrhs, double2 *A, magma_int_t lda, double2 *b, magma_int_t ldb, magma_int_t *info);

#ifdef __cplusplus
}
#endif

#endif /* _MAGMA_Z_H_ */

