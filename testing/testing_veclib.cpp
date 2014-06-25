/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       @date

       @author Mark Gates
*/

// Checks vector and matrix norms, for -m32 and -m64, with return as float or as double.
// In MacOS, -m64 must return double for both single and double precision
// functions, e.g., {s,d}dot, {s,d}nrm2, {s,d}lange, {s,d}lansy.
// Oddly, with -m32 both return float and double for single precision functions works.
// This is essentially a bug from an old f2c version of lapack (clapack).

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "magma_lapack.h"

// ------------------------------------------------------------
#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)


// ------------------------------------------------------------
#ifdef LAPACK_RETURN_DOUBLE
typedef double RETURN_FLOAT;
#else
typedef float  RETURN_FLOAT;
#endif


// ------------------------------------------------------------
#include "magma_lapack.h"


#ifdef __cplusplus
extern "C" {
#endif

RETURN_FLOAT
       sdot_(   const int *n,
                const float *x, const int *incx,
                const float *y, const int *incy );
                
RETURN_FLOAT    
       snrm2_(  const int *n,
                const float *x, const int *incx );

/*
RETURN_FLOAT
       slange_( const char *norm,
                const int *m, const int *n,
                const float *A, const int *lda,
                float *work );

RETURN_FLOAT
       slansy_( const char *norm, const char* uplo,
                const int *n,
                const float *A, const int *lda,
                float *work );
*/

double ddot_(   const int *n,
                const double *x, const int *incx,
                const double *y, const int *incy );

double dnrm2_(  const int *n,
                const double *x, const int *incx );

/*
double dlange_( const char *norm,
                const int *m, const int *n,
                const double *A, const int *lda,
                double *work );

double dlansy_( const char *norm, const char* uplo,
                const int *n,
                const double *A, const int *lda,
                double *work );
*/

#ifdef __cplusplus
}
#endif

// ------------------------------------------------------------
// call matrix norms {s,d}lan{ge,sy}.
// return value, to check that the call stack isn't messed up.
float test( int m, int n )
{
#define sA(i,j) (sA + (i) + (j)*lda)
#define dA(i,j) (dA + (i) + (j)*lda)
    
    float  *sA, *swork;
    float  snorm_one, snorm_inf, snorm_fro, snorm_max;
    
    double *dA, *dwork;
    double dnorm_one, dnorm_inf, dnorm_fro, dnorm_max;
    
    const int ione = 1;
    int lda = MAX(m,n);
    
    sA    = (float*)  malloc( lda*n * sizeof(float)  );
    dA    = (double*) malloc( lda*n * sizeof(double) );
    swork = (float*)  malloc( m     * sizeof(float)  );
    dwork = (double*) malloc( m     * sizeof(double) );
    
    for( int j = 0; j < n; ++j ) {
    for( int i = 0; i < lda; ++i ) {
        double tmp = rand() / (double)(RAND_MAX);
        *sA(i,j) = tmp;
        *dA(i,j) = tmp;
    }}
    
    double error;
    int status;
    
    // can repeat multiple times, but shows same results every time
    status = 0;
    for( int i=0; i < 1; ++i ) {
        snorm_one = sdot_(  &m, sA, &ione, sA, &ione );
        dnorm_one = ddot_(  &m, dA, &ione, dA, &ione );
        snorm_fro = snrm2_( &m, sA, &ione );
        dnorm_fro = dnrm2_( &m, dA, &ione );
        printf( "m %d, sdot %12.8f, snrm2 %12.8f\n", m, snorm_one, snorm_fro );
        printf( "m %d, ddot %12.8f, dnrm2 %12.8f\n", m, dnorm_one, dnorm_fro );
        error = fabs(snorm_one - dnorm_one) / dnorm_one;
        status |= ! (error < 1e-6);
    }
    if ( status ) {
        printf( "**** failed ****\n" );
    }
    printf( "\n" );
    
    status = 0;
    for( int i=0; i < 1; ++i ) {
        snorm_one = slange_( "one", &m, &n, sA, &lda, swork );
        snorm_inf = slange_( "inf", &m, &n, sA, &lda, swork );
        snorm_max = slange_( "max", &m, &n, sA, &lda, swork );
        snorm_fro = slange_( "fro", &m, &n, sA, &lda, swork );
                                                      
        dnorm_one = dlange_( "one", &m, &n, dA, &lda, dwork );
        dnorm_inf = dlange_( "inf", &m, &n, dA, &lda, dwork );
        dnorm_max = dlange_( "max", &m, &n, dA, &lda, dwork );
        dnorm_fro = dlange_( "fro", &m, &n, dA, &lda, dwork );
        
        printf( "m %d, n %d, slange norm one %12.8f,  inf %12.8f,  max %12.8f,  fro %12.8f\n",
                m, n, snorm_one, snorm_inf, snorm_max, snorm_fro );
        
        printf( "m %d, n %d, dlange norm one %12.8f,  inf %12.8f,  max %12.8f,  fro %12.8f\n",
                m, n, dnorm_one, dnorm_inf, dnorm_max, dnorm_fro );
        error = fabs(snorm_one - dnorm_one) / dnorm_one;
        status |= ! (error < 1e-6);
    }
    if ( status ) {
        printf( "**** failed ****\n" );
    }
    printf( "\n" );
    
    status = 0;
    for( int i=0; i < 1; ++i ) {
        snorm_one = slansy_( "one", "up", &n, sA, &lda, swork );
        snorm_inf = slansy_( "inf", "up", &n, sA, &lda, swork );
        snorm_max = slansy_( "max", "up", &n, sA, &lda, swork );
        snorm_fro = slansy_( "fro", "up", &n, sA, &lda, swork );
                                                  
        dnorm_one = dlansy_( "one", "up", &n, dA, &lda, dwork );
        dnorm_inf = dlansy_( "inf", "up", &n, dA, &lda, dwork );
        dnorm_max = dlansy_( "max", "up", &n, dA, &lda, dwork );
        dnorm_fro = dlansy_( "fro", "up", &n, dA, &lda, dwork );
        
        printf( "m %d, n %d, slansy norm one %12.8f,  inf %12.8f,  max %12.8f,  fro %12.8f\n",
                m, n, snorm_one, snorm_inf, snorm_max, snorm_fro );
        
        printf( "m %d, n %d, dlansy norm one %12.8f,  inf %12.8f,  max %12.8f,  fro %12.8f\n",
                m, n, dnorm_one, dnorm_inf, dnorm_max, dnorm_fro );
        error = fabs(snorm_one - dnorm_one) / dnorm_one;
        status |= ! (error < 1e-6);
    }
    if ( status ) {
        printf( "**** failed ****\n" );
    }
    printf( "\n" );
    
    return 1.125;
}


// ------------------------------------------------------------
int main( int argc, char** argv )
{
    int m = 100;
    int n = m;
    if ( argc > 1 ) {
        n = atoi( argv[1] );
    }
    
    float value;
    
    printf( "--------------------\n" );
    printf( "sizeof(void*) %lu, sizeof(RETURN_FLOAT) %lu\n\n",
            sizeof(void*), sizeof(RETURN_FLOAT) );
    
    // can repeat multiple times, but shows same results every time
    for( int i=0; i < 1; ++i ) {
        value = test( m, n );
        printf( "value %.4f\n\n", value );
    }
    
    return 0;
}
