/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       @date
  
       @precisions normal z -> c d s
       @author Mark Gates
*/
// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// includes, project
#include "magma_v2.h"
#include "magma_lapack.h"
#include "testings.h"


/* ////////////////////////////////////////////////////////////////////////////
   -- Testing zprint
*/
int main( int argc, char** argv)
{
    TESTING_CHECK( magma_init() );
    magma_print_environment();

    magmaDoubleComplex *hA;
    magmaDoubleComplex_ptr dA;
    //magma_int_t ione     = 1;
    //magma_int_t ISEED[4] = {0,0,0,1};
    magma_int_t M, N, lda, ldda;  //size
    magma_int_t status = 0;
    
    magma_opts opts;
    opts.parse_opts( argc, argv );

    for( int itest = 0; itest < opts.ntest; ++itest ) {
        for( int iter = 0; iter < opts.niter; ++iter ) {
            M     = opts.msize[itest];
            N     = opts.nsize[itest];
            lda   = M;
            ldda  = magma_roundup( M, opts.align );  // multiple of 32 by default
            //size  = lda*N;

            /* Allocate host memory for the matrix */
            TESTING_CHECK( magma_zmalloc_cpu( &hA, lda *N ));
            TESTING_CHECK( magma_zmalloc( &dA, ldda*N ));
        
            //lapackf77_zlarnv( &ione, ISEED, &size, hA );
            for( int j = 0; j < N; ++j ) {
                for( int i = 0; i < M; ++i ) {
                    hA[i + j*lda] = MAGMA_Z_MAKE( i + j*0.01, 0. );
                }
            }
            magma_zsetmatrix( M, N, hA, lda, dA, ldda, opts.queue );
            
            printf( "A=" );
            magma_zprint( M, N, hA, lda );
            
            printf( "dA=" );
            magma_zprint_gpu( M, N, dA, ldda );
            
            magma_free_cpu( hA );
            magma_free( dA );
        }
    }

    opts.cleanup();
    TESTING_CHECK( magma_finalize() );
    return status;
}
