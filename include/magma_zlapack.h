/*
 *   -- MAGMA (version 1.0) --
 *      Univ. of Tennessee, Knoxville
 *      Univ. of California, Berkeley
 *      Univ. of Colorado, Denver
 *      November 2010
 *
 * @precisions normal z -> s d c
 */

#ifndef MAGMA_ZLAPACK_H
#define MAGMA_ZLAPACK_H

#define PRECISION_z
#ifdef __cplusplus
extern "C" {
#endif

/* ////////////////////////////////////////////////////////////////////////////
   -- LAPACK Externs used in MAGMA
*/
#if defined(ADD_)

#    define blasf77_zaxpy      zaxpy_
#    define blasf77_zcopy      zcopy_
#    define blasf77_zdotc      zdotc_ 
#    define blasf77_zgemm      zgemm_
#    define blasf77_zgemv      zgemv_
#    define blasf77_zhemm      zhemm_
#    define blasf77_zhemv      zhemv_
#    define blasf77_zher2k     zher2k_
#    define blasf77_zherk      zherk_
#    define blasf77_zscal      zscal_
#    define blasf77_zsymm      zsymm_
#    define blasf77_zsyr2k     zsyr2k_
#    define blasf77_zsyrk      zsyrk_
#    define blasf77_zswap      zswap_
#    define blasf77_ztrmm      ztrmm_
#    define blasf77_ztrmv      ztrmv_
#    define blasf77_ztrsm      ztrsm_

#    define lapackf77_zgebd2   zgebd2_
#    define lapackf77_zgebrd   zgebrd_
#    define lapackf77_zgehd2   zgehd2_
#    define lapackf77_zgehrd   zgehrd_
#    define lapackf77_zgelqf   zgelqf_
#    define lapackf77_zgeqlf   zgeqlf_
#    define lapackf77_zgeqrf   zgeqrf_
#    define lapackf77_zgetrf   zgetrf_
#    define lapackf77_zhetd2   zhetd2_
#    define lapackf77_zhetrd   zhetrd_
#    define lapackf77_zlacpy   zlacpy_
#    define lapackf77_zlacgv   zlacgv_
#    define lapackf77_zlange   zlange_
#    define lapackf77_zlanhe   zlanhe_
#    define lapackf77_zlansy   zlansy_
#    define lapackf77_zlarfb   zlarfb_
#    define lapackf77_zlarfg   zlarfg_
#    define lapackf77_zlarft   zlarft_
#    define lapackf77_zlarnv   zlarnv_
#    define lapackf77_zlaswp   zlaswp_
#    define lapackf77_zpotrf   zpotrf_
#    define lapackf77_ztrtri   ztrtri_
#    define lapackf77_zung2r   zung2r_
#    define lapackf77_zungbr   zungbr_
#    define lapackf77_zunghr   zunghr_
#    define lapackf77_zungtr   zungtr_
#    define lapackf77_zunm2r   zunm2r_
#    define lapackf77_zunmlq   zunmlq_
#    define lapackf77_zunmql   zunmql_
#    define lapackf77_zunmqr   zunmqr_

#    define lapackf77_zbdt01   zbdt01_
#    define lapackf77_zhet21   zhet21_
#    define lapackf77_zhst01   zhst01_
#    define lapackf77_zqrt02   zqrt02_
#    define lapackf77_zunt01   zunt01_
#    define lapackf77_zlarfy   zlarfy_
#    define lapackf77_zstt21   zstt21_

#elif defined(NOCHANGE)

#    define blasf77_zaxpy      zaxpy
#    define blasf77_zcopy      zcopy
#    define blasf77_zdotc      zdotc 
#    define blasf77_zgemm      zgemm
#    define blasf77_zgemv      zgemv
#    define blasf77_zhemm      zhemm
#    define blasf77_zhemv      zhemv
#    define blasf77_zher2k     zher2k
#    define blasf77_zherk      zherk
#    define blasf77_zscal      zscal
#    define blasf77_zsymm      zsymm
#    define blasf77_zsyr2k     zsyr2k
#    define blasf77_zsyrk      zsyrk
#    define blasf77_zswap      zswap
#    define blasf77_ztrmm      ztrmm
#    define blasf77_ztrmv      ztrmv
#    define blasf77_ztrsm      ztrsm

#    define lapackf77_zgebd2   zgebd2
#    define lapackf77_zgebrd   zgebrd
#    define lapackf77_zgehd2   zgehd2
#    define lapackf77_zgehrd   zgehrd
#    define lapackf77_zgelqf   zgelqf
#    define lapackf77_zgeqlf   zgeqlf
#    define lapackf77_zgeqrf   zgeqrf
#    define lapackf77_zgetrf   zgetrf
#    define lapackf77_zhetd2   zhetd2
#    define lapackf77_zhetrd   zhetrd
#    define lapackf77_zlacpy   zlacpy
#    define lapackf77_zlacgv   zlacgv
#    define lapackf77_zlange   zlange
#    define lapackf77_zlanhe   zlanhe
#    define lapackf77_zlansy   zlansy
#    define lapackf77_zlarfb   zlarfb
#    define lapackf77_zlarfg   zlarfg
#    define lapackf77_zlarft   zlarft
#    define lapackf77_zlarnv   zlarnv
#    define lapackf77_zlaswp   zlaswp
#    define lapackf77_zpotrf   zpotrf
#    define lapackf77_ztrtri   ztrtri
#    define lapackf77_zung2r   zung2r
#    define lapackf77_zungbr   zungbr
#    define lapackf77_zunghr   zunghr
#    define lapackf77_zungtr   zungtr
#    define lapackf77_zunm2r   zunm2r
#    define lapackf77_zunmlq   zunmlq
#    define lapackf77_zunmql   zunmql
#    define lapackf77_zunmqr   zunmqr

#    define lapackf77_zbdt01   zbdt01
#    define lapackf77_zhet21   zhet21
#    define lapackf77_zhst01   zhst01
#    define lapackf77_zqrt02   zqrt02
#    define lapackf77_zunt01   zunt01
#    define lapackf77_zlarfy   zlarfy
#    define lapackf77_zstt21   zstt21

#endif

void    blasf77_zaxpy( const int *, cuDoubleComplex *, cuDoubleComplex *, const int *, cuDoubleComplex *, const int *);
void    blasf77_zcopy( const int *, cuDoubleComplex *, const int *, cuDoubleComplex *, const int *);
cuDoubleComplex blasf77_zdotc( const int *, cuDoubleComplex *, const int *, cuDoubleComplex *, const int *);
void    blasf77_zgemm( const char *, const char *, const int *, const int *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *, cuDoubleComplex *, const int *, cuDoubleComplex *,cuDoubleComplex *, const int *);
void    blasf77_zgemv( const char *, const int  *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *, cuDoubleComplex *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *);
void    blasf77_zhemm( const char *, const char *, const int *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *, cuDoubleComplex *, const int *, cuDoubleComplex *,cuDoubleComplex *, const int *);
void    blasf77_zhemv( const char *, const int  *, cuDoubleComplex *, cuDoubleComplex *, const int *, cuDoubleComplex *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *);
void    blasf77_zher2k(const char *, const char *, const int *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *, cuDoubleComplex *, const int *, double *,  cuDoubleComplex *, const int *);
void    blasf77_zherk( const char *, const char *, const int *, const int *, double  *, cuDoubleComplex *, const int *, double *, cuDoubleComplex *, const int *);
void    blasf77_zscal( const int *, cuDoubleComplex *, cuDoubleComplex *, const int *);
void    blasf77_zsymm( const char *, const char *, const int *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *, cuDoubleComplex *, const int *, cuDoubleComplex *,cuDoubleComplex *, const int *);
void    blasf77_zsyr2k(const char *, const char *, const int *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *, cuDoubleComplex *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *);
void    blasf77_zsyrk( const char *, const char *, const int *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *);
void    blasf77_zswap( int *, cuDoubleComplex *, int *, cuDoubleComplex *, int *);
void    blasf77_ztrmm( const char *, const char *, const char *, const char *, const int *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *, cuDoubleComplex *,const int *);
void    blasf77_ztrmv( const char *, const char *, const char*, const int *, cuDoubleComplex*,  const int *, cuDoubleComplex *, const int*);
void    blasf77_ztrsm( const char *, const char *, const char *, const char *, const int *, const int *, cuDoubleComplex *, cuDoubleComplex *, const int *, cuDoubleComplex *,const int*);

void    lapackf77_zgebd2(magma_int_t *m, magma_int_t *n, cuDoubleComplex *a, magma_int_t *lda, double *d, double *e, cuDoubleComplex *tauq, cuDoubleComplex *taup, cuDoubleComplex *work, magma_int_t *info);
void    lapackf77_zgebrd(magma_int_t *m, magma_int_t *n, cuDoubleComplex *a, magma_int_t *lda, double *d, double *e, cuDoubleComplex *tauq, cuDoubleComplex *taup, cuDoubleComplex *work, magma_int_t *lwork, magma_int_t *info);
void    lapackf77_zgehd2(magma_int_t *n, magma_int_t *ilo, magma_int_t *ihi, cuDoubleComplex *a, magma_int_t *lda, cuDoubleComplex *tau, cuDoubleComplex *work, magma_int_t *info);
void    lapackf77_zgehrd(magma_int_t *n, magma_int_t *ilo, magma_int_t *ihi, cuDoubleComplex *a, magma_int_t *lda, cuDoubleComplex *tau, cuDoubleComplex *work, magma_int_t *lwork, magma_int_t *info);
void    lapackf77_zgelqf(magma_int_t *m, magma_int_t *n, cuDoubleComplex *a, magma_int_t *lda, cuDoubleComplex *tau, cuDoubleComplex *work, magma_int_t *lwork, magma_int_t *info);
void    lapackf77_zgeqlf(magma_int_t *m, magma_int_t *n, cuDoubleComplex *a, magma_int_t *lda, cuDoubleComplex *tau, cuDoubleComplex *work, magma_int_t *lwork, magma_int_t *info);
void    lapackf77_zgeqrf(magma_int_t *m, magma_int_t *n, cuDoubleComplex *a, magma_int_t *lda, cuDoubleComplex *tau, cuDoubleComplex *work, magma_int_t *lwork, magma_int_t *info);
void    lapackf77_zgetrf(magma_int_t *m, magma_int_t *n, cuDoubleComplex *a, magma_int_t *lda, magma_int_t *ipiv, magma_int_t *info);
void    lapackf77_zhetd2(const char *uplo, magma_int_t *n, cuDoubleComplex *a, magma_int_t *lda, double *d, double *e, cuDoubleComplex *tau, magma_int_t *info);
void    lapackf77_zhetrd(const char *uplo, magma_int_t *n, cuDoubleComplex *a, magma_int_t *lda, double *d, double *e, cuDoubleComplex *tau, cuDoubleComplex *work, magma_int_t *lwork, magma_int_t *info);
void    lapackf77_zlacpy(const char *uplo, magma_int_t *m, magma_int_t *n, const cuDoubleComplex *a, magma_int_t *lda, cuDoubleComplex *b, magma_int_t *ldb);
void    lapackf77_zlacgv(magma_int_t *n, cuDoubleComplex *x, magma_int_t *incx);
double  lapackf77_zlange(const char *norm, magma_int_t *m, magma_int_t *n, const cuDoubleComplex *a, magma_int_t *lda, double *work);
double  lapackf77_zlanhe(const char *norm, const char *uplo, magma_int_t *n, const cuDoubleComplex *a, magma_int_t *lda, double * work);
double  lapackf77_zlansy(const char *norm, const char *uplo, magma_int_t *n, const cuDoubleComplex *a, magma_int_t *lda, double * work);
void    lapackf77_zlarfb(const char *side, const char *trans, const char *direct, const char *storev, magma_int_t *m, magma_int_t *n, magma_int_t *k, const cuDoubleComplex *v, magma_int_t *ldv, const cuDoubleComplex *t, magma_int_t *ldt, cuDoubleComplex *c, magma_int_t *ldc, cuDoubleComplex *work, magma_int_t *ldwork);
void    lapackf77_zlarfg(magma_int_t *n, cuDoubleComplex *alpha, cuDoubleComplex *x, magma_int_t *incx, cuDoubleComplex *tau);
void    lapackf77_zlarft(const char *direct, const char *storev, magma_int_t *n, magma_int_t *k, cuDoubleComplex *v, magma_int_t *ldv, const cuDoubleComplex *tau, cuDoubleComplex *t, magma_int_t *ldt);
void    lapackf77_zlarnv(magma_int_t *idist, magma_int_t *iseed, magma_int_t *n, cuDoubleComplex *x);
void    lapackf77_zlaswp(magma_int_t *n, cuDoubleComplex *a, magma_int_t *lda, magma_int_t *k1, magma_int_t *k2, magma_int_t *ipiv, magma_int_t *incx);
void    lapackf77_zpotrf(const char *uplo, magma_int_t *n, cuDoubleComplex *a, magma_int_t *lda, magma_int_t *info);
void    lapackf77_ztrtri(const char *uplo, const char *diag, magma_int_t *n, cuDoubleComplex *a, magma_int_t *lda, magma_int_t *info);
void    lapackf77_zung2r(magma_int_t *m, magma_int_t *n, magma_int_t *k, cuDoubleComplex *a, magma_int_t *lda, const cuDoubleComplex *tau, cuDoubleComplex *work, magma_int_t *info);
void    lapackf77_zungbr(const char *vect, magma_int_t *m, magma_int_t *n, magma_int_t *k, cuDoubleComplex *a, magma_int_t *lda, const cuDoubleComplex *tau, cuDoubleComplex *work, magma_int_t *lwork, magma_int_t *info);
void    lapackf77_zunghr(magma_int_t *n, magma_int_t *ilo, magma_int_t *ihi, cuDoubleComplex *a, magma_int_t *lda, const cuDoubleComplex *tau, cuDoubleComplex *work, magma_int_t *lwork, magma_int_t *info);
void    lapackf77_zungtr(const char *uplo, magma_int_t *n, cuDoubleComplex *a, magma_int_t *lda, const cuDoubleComplex *tau, cuDoubleComplex *work, magma_int_t *lwork, magma_int_t *info);
void    lapackf77_zunm2r(const char *side, const char *trans, magma_int_t *m, magma_int_t *n, magma_int_t *k, const cuDoubleComplex *a, magma_int_t *lda, const cuDoubleComplex *tau, cuDoubleComplex *c, magma_int_t *ldc, cuDoubleComplex *work, magma_int_t *info);
void    lapackf77_zunmlq(const char *side, const char *trans, magma_int_t *m, magma_int_t *n, magma_int_t *k, const cuDoubleComplex *a, magma_int_t *lda, const cuDoubleComplex *tau, cuDoubleComplex *c, magma_int_t *ldc, cuDoubleComplex *work, magma_int_t *lwork, magma_int_t *info);
void    lapackf77_zunmql(const char *side, const char *trans, magma_int_t *m, magma_int_t *n, magma_int_t *k, const cuDoubleComplex *a, magma_int_t *lda, const cuDoubleComplex *tau, cuDoubleComplex *c, magma_int_t *ldc, cuDoubleComplex *work, magma_int_t *lwork, magma_int_t *info);
void    lapackf77_zunmqr(const char *side, const char *trans, magma_int_t *m, magma_int_t *n, magma_int_t *k, const cuDoubleComplex *a, magma_int_t *lda, const cuDoubleComplex *tau, cuDoubleComplex *c, magma_int_t *ldc, cuDoubleComplex *work, magma_int_t *lwork, magma_int_t *info);

  /*
   * Testing functions
   */
#if defined(PRECISION_z) || defined(PRECISION_c)
void    lapackf77_zbdt01(int *m, int *n, int *kd, cuDoubleComplex *A, int *lda, cuDoubleComplex *Q, int *ldq, double *D, double *E, cuDoubleComplex *PT, int *ldpt, cuDoubleComplex *work, double *rwork, double *resid);
void    lapackf77_zhet21(int *itype, const char *uplo, int *n, int *kband, cuDoubleComplex *A, int *lda, double *D, double *E, cuDoubleComplex *U, int *ldu, cuDoubleComplex *V, int *ldv, cuDoubleComplex *TAU, cuDoubleComplex *work, double *rwork, double *result);
 void    lapackf77_zhst01(int *n, int *ilo, int *ihi, cuDoubleComplex *A, int *lda, cuDoubleComplex *H, int *ldh, cuDoubleComplex *Q, int *ldq, cuDoubleComplex *work, int *lwork, double *rwork, double *result);
void    lapackf77_zstt21(int *n, int *kband, double *AD, double *AE, double *SD, double *SE, cuDoubleComplex *U, int *ldu, cuDoubleComplex *work, double *rwork, double *result);
void    lapackf77_zunt01(const char *rowcol, int *m, int *n, cuDoubleComplex *U, int *ldu, cuDoubleComplex *work, int *lwork, double *rwork, double *resid);
#else
void    lapackf77_zbdt01(int *m, int *n, int *kd, cuDoubleComplex *A, int *lda, cuDoubleComplex *Q, int *ldq, double *D, double *E, cuDoubleComplex *PT, int *ldpt, cuDoubleComplex *work, double *resid);
void    lapackf77_zhet21(int *itype, const char *uplo, int *n, int *kband, cuDoubleComplex *A, int *lda, double *D, double *E, cuDoubleComplex *U, int *ldu, cuDoubleComplex *V, int *ldv, cuDoubleComplex *TAU, cuDoubleComplex *work, double *result);
void    lapackf77_zhst01(int *n, int *ilo, int *ihi, cuDoubleComplex *A, int *lda, cuDoubleComplex *H, int *ldh, cuDoubleComplex *Q, int *ldq, cuDoubleComplex *work, int *lwork, double *result);
void    lapackf77_zstt21(int *n, int *kband, double *AD, double *AE, double *SD, double *SE, cuDoubleComplex *U, int *ldu, cuDoubleComplex *work, double *result);
void    lapackf77_zunt01(const char *rowcol, int *m, int *n, cuDoubleComplex *U, int *ldu, cuDoubleComplex *work, int *lwork, double *resid);
#endif
void    lapackf77_zlarfy(const char *uplo, int *N, cuDoubleComplex *V, int *incv, cuDoubleComplex *tau, cuDoubleComplex *C, int *ldc, cuDoubleComplex *work);
void    lapackf77_zqrt02(int *m, int *n, int *k, cuDoubleComplex *A, cuDoubleComplex *AF, cuDoubleComplex *Q, cuDoubleComplex *R, int *lda, cuDoubleComplex *TAU, cuDoubleComplex *work, int *lwork, double *rwork, double *result);

#ifdef __cplusplus
}
#endif

#undef PRECISION_z
#endif /* MAGMA ZLAPACK */
