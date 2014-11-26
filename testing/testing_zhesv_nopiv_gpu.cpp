/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       @date

       @precisions normal z -> c d s
       
       @author Mark Gates
       @author Adrien Remy
*/
// includes, system
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// includes, project
#include "flops.h"
#include "magma.h"
#include "magma_lapack.h"
#include "testings.h"

/* ////////////////////////////////////////////////////////////////////////////
   -- Testing zhesv_nopiv_gpu
*/
int main(int argc, char **argv)
{
    TESTING_INIT();

    real_Double_t   gflops, cpu_perf, cpu_time, gpu_perf, gpu_time;
    double          error, Rnorm, Anorm, Xnorm, *work;
    magmaDoubleComplex c_one     = MAGMA_Z_ONE;
    magmaDoubleComplex c_neg_one = MAGMA_Z_NEG_ONE;
    magmaDoubleComplex *h_A, *h_B, *h_X, temp, *cwork;
    magmaDoubleComplex_ptr d_A, d_B;
    magma_int_t *ipiv;
    magma_int_t N, nrhs, lda, ldb, ldda, lddb, info, sizeA, sizeB;
    magma_int_t ione     = 1;
    magma_int_t ISEED[4] = {0,0,0,1};
    magma_int_t status = 0;
    
    magma_opts opts;
    parse_opts( argc, argv, &opts );
    
    double tol = opts.tolerance * lapackf77_dlamch("E");
    
    nrhs = opts.nrhs;
    
    printf("    N  NRHS   CPU GFlop/s (sec)   GPU GFlop/s (sec)   ||B - AX|| / N*||A||*||X||\n");
    printf("================================================================================\n");
    for( int itest = 0; itest < opts.ntest; ++itest ) {
        for( int iter = 0; iter < opts.niter; ++iter ) {
            N = opts.nsize[itest];
            lda    = N;
            ldb    = lda;
            ldda   = ((N+31)/32)*32;
            lddb   = ldda;
            gflops = ( FLOPS_ZGETRF( N, N ) + FLOPS_ZGETRS( N, nrhs ) ) / 1e9;
            
            TESTING_MALLOC_CPU( h_A, magmaDoubleComplex, lda*N    );
            TESTING_MALLOC_CPU( h_B, magmaDoubleComplex, ldb*nrhs );
            TESTING_MALLOC_CPU( h_X, magmaDoubleComplex, ldb*nrhs );
            TESTING_MALLOC_CPU( work, double,      N );
            TESTING_MALLOC_CPU( ipiv, magma_int_t, N );
            
            TESTING_MALLOC_DEV( d_A, magmaDoubleComplex, ldda*N    );
            TESTING_MALLOC_DEV( d_B, magmaDoubleComplex, lddb*nrhs );
            
            /* Initialize the matrices */
            sizeA = lda*N;
            sizeB = ldb*nrhs;
            lapackf77_zlarnv( &ione, ISEED, &sizeA, h_A );
            lapackf77_zlarnv( &ione, ISEED, &sizeB, h_B );
           
            magma_int_t nopiv = 1;

            for (int i=0; i<N; i++) for (int j=0; j<i; j++) h_A[i+j*lda] = MAGMA_Z_CNJG(h_A[j+i*lda]);
            if (nopiv) for (int i=0; i<N; i++) h_A[i+i*lda] = MAGMA_Z_MAKE(MAGMA_Z_REAL(h_A[i+i*lda]) + N, 0.0);
            else       for (int i=0; i<N; i++) h_A[i+i*lda] = MAGMA_Z_MAKE(MAGMA_Z_REAL(h_A[i+i*lda]), 0.0);

            magma_uplo_t uplo = opts.uplo;

            magma_int_t upper = (uplo == MagmaUpper);

            magma_zsetmatrix( N, N,    h_A, lda, d_A, ldda );
            magma_zsetmatrix( N, nrhs, h_B, ldb, d_B, lddb );
            
            /* ====================================================================
               Performs operation using MAGMA
               =================================================================== */
            gpu_time = magma_wtime();

            magma_zhesv_nopiv_gpu( uplo, N, nrhs, d_A, ldda, d_B, lddb, &info );
            gpu_time = magma_wtime() - gpu_time;
            gpu_perf = gflops / gpu_time;
            if (info != 0)
                printf("magma_zgesv_gpu returned error %d: %s.\n",
                       (int) info, magma_strerror( info ));
            
            //=====================================================================
            // Residual
            //=====================================================================
            magma_zgetmatrix( N, nrhs, d_B, lddb, h_X, ldb );
            
            Anorm = lapackf77_zlange("I", &N, &N,    h_A, &lda, work);
            Xnorm = lapackf77_zlange("I", &N, &nrhs, h_X, &ldb, work);
            
            blasf77_zgemm( MagmaNoTransStr, MagmaNoTransStr, &N, &nrhs, &N,
                           &c_one,     h_A, &lda,
                                       h_X, &ldb,
                           &c_neg_one, h_B, &ldb);
            
            Rnorm = lapackf77_zlange("I", &N, &nrhs, h_B, &ldb, work);
            error = Rnorm/(N*Anorm*Xnorm);
            status += ! (error < tol);
            
            /* ====================================================================
               Performs operation using LAPACK
               =================================================================== */
            if ( opts.lapack ) {
                int lwork = -1;
                lapackf77_zhesv((upper ? MagmaUpperStr: MagmaLowerStr), &N,&nrhs, h_A, &lda, ipiv, h_B, &ldb, &temp, &lwork, &info);
                lwork = (int)MAGMA_Z_REAL(temp);
                TESTING_MALLOC_PIN( cwork, magmaDoubleComplex, lwork );

                cpu_time = magma_wtime();
                lapackf77_zhesv((upper ? MagmaUpperStr: MagmaLowerStr), &N, &nrhs, h_A, &lda, ipiv, h_B, &ldb, cwork, &lwork, &info);
                cpu_time = magma_wtime() - cpu_time;
                cpu_perf = gflops / cpu_time;
                if (info != 0)
                    printf("lapackf77_zhesv returned error %d: %s.\n",
                           (int) info, magma_strerror( info ));
                
                printf( "%5d %5d   %7.2f (%7.2f)   %7.2f (%7.2f)   %8.2e   %s\n",
                        (int) N, (int) nrhs, cpu_perf, cpu_time, gpu_perf, gpu_time,
                        error, (error < tol ? "ok" : "failed"));
                TESTING_FREE_CPU(cwork);
            }
            else {
                printf( "%5d %5d     ---   (  ---  )   %7.2f (%7.2f)   %8.2e   %s\n",
                        (int) N, (int) nrhs, gpu_perf, gpu_time,
                        error, (error < tol ? "ok" : "failed"));
            }
            
            TESTING_FREE_CPU( h_A );
            TESTING_FREE_CPU( h_B );
            TESTING_FREE_CPU( h_X );
            TESTING_FREE_CPU( work );
            TESTING_FREE_CPU( ipiv );
            
            TESTING_FREE_DEV( d_A );
            TESTING_FREE_DEV( d_B );
            fflush( stdout );
        }
        if ( opts.niter > 1 ) {
            printf( "\n" );
        }
    }

    TESTING_FINALIZE();
    return status;
}
