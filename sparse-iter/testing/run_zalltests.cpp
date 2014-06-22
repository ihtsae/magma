/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       @date

       @precisions normal z -> s d c
       @author Hartwig Anzt
*/

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <cuda_runtime_api.h>
#include <cublas.h>

// includes, project
#include "flops.h"
#include "magma.h"
#include "magma_lapack.h"
#include "testings.h"
#include "../include/magmasparse.h"

/* ////////////////////////////////////////////////////////////////////////////
   -- Debugging file
*/
int main( int argc, char** argv)
{
    TESTING_INIT();

    magma_z_solver_par solver_par;
    magma_z_preconditioner precond_par;
    solver_par.epsilon = 10e-16;
    solver_par.maxiter = 1000;
    solver_par.verbose = 0;
    solver_par.restart = 30;
    solver_par.num_eigenvalues = 0;
    solver_par.ortho = Magma_CGS;
    
    magmaDoubleComplex one = MAGMA_Z_MAKE(1.0, 0.0);
    magmaDoubleComplex zero = MAGMA_Z_MAKE(0.0, 0.0);

    magma_z_sparse_matrix A, B, B_d;
    magma_z_vector x, b;

    // generate matrix of desired structure and size
    magma_int_t n=100;   // size is n*n
    magma_int_t nn = n*n;
    magma_int_t offdiags = 2;
    magma_index_t *diag_offset;
    magmaDoubleComplex *diag_vals;
    magma_zmalloc_cpu( &diag_vals, offdiags+1 );
    magma_index_malloc_cpu( &diag_offset, offdiags+1 );
    diag_offset[0] = 0;
    diag_offset[1] = 1;
    diag_offset[2] = n;
    diag_vals[0] = MAGMA_Z_MAKE( 4.1, 0.0 );
    diag_vals[1] = MAGMA_Z_MAKE( -1.0, 0.0 );
    diag_vals[2] = MAGMA_Z_MAKE( -1.0, 0.0 );
    magma_zmgenerator( nn, offdiags, diag_offset, diag_vals, &A );

    // convert marix into desired format
    B.storage_type = Magma_SELLC;
    B.blocksize = 8;
    B.alignment = 8;
    // scale matrix
    magma_zmscale( &A, Magma_UNITDIAG );

    magma_z_mconvert( A, &B, Magma_CSR, B.storage_type );
    magma_z_mtransfer( B, &B_d, Magma_CPU, Magma_DEV );


    // test CG ####################################
    // vectors and initial guess
    magma_z_vinit( &b, Magma_DEV, A.num_cols, one );
    magma_z_vinit( &x, Magma_DEV, A.num_cols, one );
    magma_z_spmv( one, B_d, x, zero, b );                 //  b = A x
    magma_z_vfree(&x);
    magma_z_vinit( &x, Magma_DEV, A.num_cols, zero );
    magma_zsolverinfo_init( &solver_par, &precond_par );
    // solver
    magma_zcg_res( B_d, b, &x, &solver_par );
    // solverinfo
    magma_zsolverinfo( &solver_par, &precond_par );
    if( solver_par.numiter > 150 ){
        printf("error: test not passed!\n"); exit(-1);
    }
    magma_zsolverinfo_free( &solver_par, &precond_par );
    magma_z_vfree(&x);
    magma_z_vfree(&b);

    // test PCG Jacobi ############################
    // vectors and initial guess
    magma_z_vinit( &b, Magma_DEV, A.num_cols, one );
    magma_z_vinit( &x, Magma_DEV, A.num_cols, one );
    magma_z_spmv( one, B_d, x, zero, b );                 //  b = A x
    magma_z_vfree(&x);
    magma_z_vinit( &x, Magma_DEV, A.num_cols, zero );
    magma_zsolverinfo_init( &solver_par, &precond_par );
    // Preconditioner
    precond_par.solver = Magma_JACOBI;
    magma_z_precondsetup( B_d, b, &precond_par );
    // solver
    magma_zpcg( B_d, b, &x, &solver_par, &precond_par );
    // solverinfo
    magma_zsolverinfo( &solver_par, &precond_par );
    if( solver_par.numiter > 150 ){
        printf("error: test not passed!\n"); exit(-1);
    }
    magma_zsolverinfo_free( &solver_par, &precond_par );
    magma_z_vfree(&x);
    magma_z_vfree(&b);

    // test PCG IC ################################
    // vectors and initial guess
    magma_z_vinit( &b, Magma_DEV, A.num_cols, one );
    magma_z_vinit( &x, Magma_DEV, A.num_cols, one );
    magma_z_spmv( one, B_d, x, zero, b );                 //  b = A x
    magma_z_vfree(&x);
    magma_z_vinit( &x, Magma_DEV, A.num_cols, zero );
    magma_zsolverinfo_init( &solver_par, &precond_par );
    // Preconditioner
    precond_par.solver = Magma_ICC;
    magma_z_precondsetup( B_d, b, &precond_par );
    // solver
    magma_zpcg( B_d, b, &x, &solver_par, &precond_par );
    // solverinfo
    magma_zsolverinfo( &solver_par, &precond_par );
    if( solver_par.numiter > 150 ){
        printf("error: test not passed!\n"); exit(-1);
    }
    magma_zsolverinfo_free( &solver_par, &precond_par );
    magma_z_vfree(&x);
    magma_z_vfree(&b);


    // test PCG IC ################################
    // vectors and initial guess
    magma_z_vinit( &b, Magma_DEV, A.num_cols, one );
    magma_z_vinit( &x, Magma_DEV, A.num_cols, one );
    magma_z_spmv( one, B_d, x, zero, b );                 //  b = A x
    magma_z_vfree(&x);
    magma_z_vinit( &x, Magma_DEV, A.num_cols, zero );
    magma_zsolverinfo_init( &solver_par, &precond_par );
    // Preconditioner
    precond_par.solver = Magma_ICC;
    magma_z_precondsetup( B_d, b, &precond_par );
    // solver
    magma_zpcg( B_d, b, &x, &solver_par, &precond_par );
    // solverinfo
    magma_zsolverinfo( &solver_par, &precond_par );
    if( solver_par.numiter > 150 ){
        printf("error: test not passed!\n"); exit(-1);
    }
    magma_zsolverinfo_free( &solver_par, &precond_par );
    magma_z_vfree(&x);
    magma_z_vfree(&b);

    // test BICGSTAB ####################################
    // vectors and initial guess
    magma_z_vinit( &b, Magma_DEV, A.num_cols, one );
    magma_z_vinit( &x, Magma_DEV, A.num_cols, one );
    magma_z_spmv( one, B_d, x, zero, b );                 //  b = A x
    magma_z_vfree(&x);
    magma_z_vinit( &x, Magma_DEV, A.num_cols, zero );
    magma_zsolverinfo_init( &solver_par, &precond_par );
    // solver
    magma_zbicgstab( B_d, b, &x, &solver_par );
    // solverinfo
    magma_zsolverinfo( &solver_par, &precond_par );
    if( solver_par.numiter > 150 ){
        printf("error: test not passed!\n"); exit(-1);
    }
    magma_zsolverinfo_free( &solver_par, &precond_par );
    magma_z_vfree(&x);
    magma_z_vfree(&b);

    // test PBICGSTAB Jacobi ############################
    // vectors and initial guess
    magma_z_vinit( &b, Magma_DEV, A.num_cols, one );
    magma_z_vinit( &x, Magma_DEV, A.num_cols, one );
    magma_z_spmv( one, B_d, x, zero, b );                 //  b = A x
    magma_z_vfree(&x);
    magma_z_vinit( &x, Magma_DEV, A.num_cols, zero );
    magma_zsolverinfo_init( &solver_par, &precond_par );
    // Preconditioner
    precond_par.solver = Magma_JACOBI;
    magma_z_precondsetup( B_d, b, &precond_par );
    // solver
    magma_zpbicgstab( B_d, b, &x, &solver_par, &precond_par );
    // solverinfo
    magma_zsolverinfo( &solver_par, &precond_par );
    if( solver_par.numiter > 150 ){
        printf("error: test not passed!\n"); exit(-1);
    }
    magma_zsolverinfo_free( &solver_par, &precond_par );
    magma_z_vfree(&x);
    magma_z_vfree(&b);
/*
    // test PBICGSTAB ILU ###############################
    // vectors and initial guess
    magma_z_vinit( &b, Magma_DEV, A.num_cols, one );
    magma_z_vinit( &x, Magma_DEV, A.num_cols, one );
    magma_z_spmv( one, B_d, x, zero, b );                 //  b = A x
    magma_z_vfree(&x);
    magma_z_vinit( &x, Magma_DEV, A.num_cols, zero );
    magma_zsolverinfo_init( &solver_par, &precond_par );
    // Preconditioner
    precond_par.solver = Magma_ILU;
    magma_z_precondsetup( B_d, b, &precond_par );
    // solver
    magma_zpbicgstab( B_d, b, &x, &solver_par, &precond_par );
    // solverinfo
    magma_zsolverinfo( &solver_par, &precond_par );
    if( solver_par.numiter > 150 ){
        printf("error: test not passed!\n"); exit(-1);
    }
    magma_zsolverinfo_free( &solver_par, &precond_par );
    magma_z_vfree(&x);
    magma_z_vfree(&b);

    // test PBICGSTAB ILU ###############################
    // vectors and initial guess
    magma_z_vinit( &b, Magma_DEV, A.num_cols, one );
    magma_z_vinit( &x, Magma_DEV, A.num_cols, one );
    magma_z_spmv( one, B_d, x, zero, b );                 //  b = A x
    magma_z_vfree(&x);printf("here\n");
    magma_z_vinit( &x, Magma_DEV, A.num_cols, zero );
    magma_zsolverinfo_init( &solver_par, &precond_par );
    // Preconditioner
    precond_par.solver = Magma_ILU;
    magma_z_precondsetup( B_d, b, &precond_par );
    // solver
    magma_zpbicgstab( B_d, b, &x, &solver_par, &precond_par );
    // solverinfo
    magma_zsolverinfo( &solver_par, &precond_par );
    if( solver_par.numiter > 150 ){
        printf("error: test not passed!\n"); exit(-1);
    }
    magma_zsolverinfo_free( &solver_par, &precond_par );
    magma_z_vfree(&x);
    magma_z_vfree(&b);

    // test GMRES ####################################
    // vectors and initial guess
    magma_z_vinit( &b, Magma_DEV, A.num_cols, one );
    magma_z_vinit( &x, Magma_DEV, A.num_cols, one );
    magma_z_spmv( one, B_d, x, zero, b );                 //  b = A x
    magma_z_vfree(&x);
    magma_z_vinit( &x, Magma_DEV, A.num_cols, zero );
    magma_zsolverinfo_init( &solver_par, &precond_par );
    // solver
    magma_zgmres( B_d, b, &x, &solver_par );
    // solverinfo
    magma_zsolverinfo( &solver_par, &precond_par );
    magma_zsolverinfo_free( &solver_par, &precond_par );
    magma_z_vfree(&x);
    magma_z_vfree(&b);

    // test PGMRES Jacobi ############################
    // vectors and initial guess
    magma_z_vinit( &b, Magma_DEV, A.num_cols, one );
    magma_z_vinit( &x, Magma_DEV, A.num_cols, one );
    magma_z_spmv( one, B_d, x, zero, b );                 //  b = A x
    magma_z_vfree(&x);
    magma_z_vinit( &x, Magma_DEV, A.num_cols, zero );
    magma_zsolverinfo_init( &solver_par, &precond_par );
    // Preconditioner
    precond_par.solver = Magma_JACOBI;
    magma_z_precondsetup( B_d, b, &precond_par );
    // solver
    magma_zpgmres( B_d, b, &x, &solver_par, &precond_par );
    // solverinfo
    magma_zsolverinfo( &solver_par, &precond_par );
    magma_zsolverinfo_free( &solver_par, &precond_par );
    magma_z_vfree(&x);
    magma_z_vfree(&b);*/

    // test PGMRES ILU ###############################
    // vectors and initial guess
    magma_z_vinit( &b, Magma_DEV, A.num_cols, one );
    magma_z_vinit( &x, Magma_DEV, A.num_cols, one );
    magma_z_spmv( one, B_d, x, zero, b );                 //  b = A x
    magma_z_vfree(&x);
    magma_z_vinit( &x, Magma_DEV, A.num_cols, zero );
    magma_zsolverinfo_init( &solver_par, &precond_par );
    // Preconditioner
    precond_par.solver = Magma_ILU;
    magma_z_precondsetup( B_d, b, &precond_par );
    // solver
    magma_zpgmres( B_d, b, &x, &solver_par, &precond_par );
    // solverinfo
    magma_zsolverinfo( &solver_par, &precond_par );
    if( solver_par.numiter > 150 ){
        printf("error: test not passed!\n"); exit(-1);
    }
    magma_zsolverinfo_free( &solver_par, &precond_par );
    magma_z_vfree(&x);
    magma_z_vfree(&b);


    printf("all tests passed.\n");




    magma_z_mfree(&B_d);
    magma_z_mfree(&B);
    magma_z_mfree(&A); 


    TESTING_FINALIZE();
    return 0;
}
