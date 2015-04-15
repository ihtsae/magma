/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       @date

       @precisions normal z -> s d c
       @author Hartwig Anzt

*/

// includes, project
#include "common_magma.h"
#include "magmasparse_z.h"
#include "magma.h"
#include "mmio.h"
#include "magma_lapack.h"




/**
    Purpose
    -------

    Removes zeros in a CSR matrix.

    Arguments
    ---------

    @param[in,out]
    A           magma_z_matrix*
                input/output matrix 
    @param[in]
    queue       magma_queue_t
                Queue to execute in.

    @ingroup magmasparse_zaux
    ********************************************************************/

extern "C" magma_int_t
magma_zmcsrcompressor(
    magma_z_matrix *A,
    magma_queue_t queue )
{
    if ( A->memory_location == Magma_CPU && A->storage_type == Magma_CSR ) {

        magma_z_matrix B;

        magma_zmconvert( *A, &B, Magma_CSR, Magma_CSR, queue );

        magma_free_cpu( A->row );
        magma_free_cpu( A->col );
        magma_free_cpu( A->val );
        magma_z_csr_compressor(&B.val, &B.row, &B.col, 
                       &A->val, &A->row, &A->col, &A->num_rows, queue );  
        A->nnz = A->row[A->num_rows];

        magma_zmfree( &B, queue );       

        return MAGMA_SUCCESS; 
    }
    else {

        magma_z_matrix hA, CSRA;
        magma_storage_t A_storage = A->storage_type;
        magma_location_t A_location = A->memory_location;
        magma_zmtransfer( *A, &hA, A->memory_location, Magma_CPU, queue );
        magma_zmconvert( hA, &CSRA, hA.storage_type, Magma_CSR, queue );

        magma_zmcsrcompressor( &CSRA, queue );

        magma_zmfree( &hA, queue );
        magma_zmfree( A, queue );
        magma_zmconvert( CSRA, &hA, Magma_CSR, A_storage, queue );
        magma_zmtransfer( hA, A, Magma_CPU, A_location, queue );
        magma_zmfree( &hA, queue );
        magma_zmfree( &CSRA, queue );    

        return MAGMA_SUCCESS; 
    }
}


