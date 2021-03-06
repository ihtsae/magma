/*******************************************************************************
 *  This file is AUTOMATICALLY GENERATED by:
 *  ../tools/fortran_wrappers.pl ../include/magmablas_ds.i
 *  Do not edit.
 ******************************************************************************/

#include <stdint.h>  // for uintptr_t

#include "magma_v2.h"
#include "magma_mangling.h"

/*
 * typedef comming from fortran.h file provided in CUDADIR/src directory
 * it will probably change with future release of CUDA when they use 64 bit addresses
 */
typedef size_t devptr_t;

#ifdef PGI_FORTRAN
    #define magma_idevptr(ptr_) ((magma_int_t*)       (ptr_))
    #define magma_zdevptr(ptr_) ((magmaDoubleComplex*)(ptr_))
    #define magma_cdevptr(ptr_) ((magmaFloatComplex*) (ptr_))
    #define magma_ddevptr(ptr_) ((double*)            (ptr_))
    #define magma_sdevptr(ptr_) ((float*)             (ptr_))
#else
    #define magma_idevptr(ptr_) ((magma_int_t*)       (uintptr_t)(*(ptr_)))
    #define magma_zdevptr(ptr_) ((magmaDoubleComplex*)(uintptr_t)(*(ptr_)))
    #define magma_cdevptr(ptr_) ((magmaFloatComplex*) (uintptr_t)(*(ptr_)))
    #define magma_ddevptr(ptr_) ((double*)            (uintptr_t)(*(ptr_)))
    #define magma_sdevptr(ptr_) ((float*)             (uintptr_t)(*(ptr_)))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define magmablasf_dsaxpycp FORTRAN_NAME( magmablasf_dsaxpycp, MAGMABLASF_DSAXPYCP )
void magmablasf_dsaxpycp(
    magma_int_t *m,
    devptr_t *r,
    devptr_t *x,
    devptr_t *b,
    devptr_t *w )
{
    magmablas_dsaxpycp(
        *m,
        magma_sdevptr(r),
        magma_ddevptr(x),
        magma_ddevptr(b),
        magma_ddevptr(w) );
}

#define magmablasf_dslaswp FORTRAN_NAME( magmablasf_dslaswp, MAGMABLASF_DSLASWP )
void magmablasf_dslaswp(
    magma_int_t *n,
    devptr_t *A, magma_int_t *lda,
    devptr_t *SA, magma_int_t *m,
    magma_int_t *ipiv, magma_int_t *incx )
{
    magmablas_dslaswp(
        *n,
        magma_ddevptr(A), *lda,
        magma_sdevptr(SA), *m,
        ipiv, *incx );
}

#define magmablasf_dlag2s FORTRAN_NAME( magmablasf_dlag2s, MAGMABLASF_DLAG2S )
void magmablasf_dlag2s(
    magma_int_t *m, magma_int_t *n,
    devptr_t *A, magma_int_t *lda,
    devptr_t *SA, magma_int_t *ldsa,
    magma_int_t *info )
{
    magmablas_dlag2s(
        *m, *n,
        magma_ddevptr(A), *lda,
        magma_sdevptr(SA), *ldsa,
        info );
}

#define magmablasf_slag2d FORTRAN_NAME( magmablasf_slag2d, MAGMABLASF_SLAG2D )
void magmablasf_slag2d(
    magma_int_t *m, magma_int_t *n,
    devptr_t *SA, magma_int_t *ldsa,
    devptr_t *A, magma_int_t *lda,
    magma_int_t *info )
{
    magmablas_slag2d(
        *m, *n,
        magma_sdevptr(SA), *ldsa,
        magma_ddevptr(A), *lda,
        info );
}

#define magmablasf_dlat2s FORTRAN_NAME( magmablasf_dlat2s, MAGMABLASF_DLAT2S )
void magmablasf_dlat2s(
    const char* uplo, magma_int_t *n,
    devptr_t *A, magma_int_t *lda,
    devptr_t *SA, magma_int_t *ldsa,
    magma_int_t *info )
{
    magmablas_dlat2s(
        magma_uplo_const(*uplo), *n,
        magma_ddevptr(A), *lda,
        magma_sdevptr(SA), *ldsa,
        info );
}

#define magmablasf_slat2d FORTRAN_NAME( magmablasf_slat2d, MAGMABLASF_SLAT2D )
void magmablasf_slat2d(
    const char* uplo, magma_int_t *n,
    devptr_t *SA, magma_int_t *ldsa,
    devptr_t *A, magma_int_t *lda,
    magma_int_t *info )
{
    magmablas_slat2d(
        magma_uplo_const(*uplo), *n,
        magma_sdevptr(SA), *ldsa,
        magma_ddevptr(A), *lda,
        info );
}


#ifdef __cplusplus
}
#endif
