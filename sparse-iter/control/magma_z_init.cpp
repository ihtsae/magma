/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2011

       @precisions normal z -> s d c
       @author Hartwig Anzt
*/

#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <ostream>
#include <assert.h>
#include <stdio.h>
#include "../include/magmasparse_z.h"
#include "../../include/magma.h"
#include "../include/mmio.h"



using namespace std;








/*  -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2011

    Purpose
    =======

    Initialize a magma_z_vector.


    Arguments
    =========

    magma_z_vector x                     vector to initialize   
    magma_location_t memory_location     memory for vector 
    magma_int_t num_rows                 desired length of vector      
    magmaDoubleComplex values            entries in vector

    ========================================================================  */

magma_int_t 
magma_z_vinit(    magma_z_vector *x, 
                  magma_location_t memory_location,
                  magma_int_t num_rows, 
                  magmaDoubleComplex values ){

    x->memory_location = Magma_CPU;
    x->num_rows = num_rows;
    x->nnz = num_rows;
    if( memory_location == Magma_CPU ){
        x->memory_location = Magma_CPU;

        magma_zmalloc_cpu( &x->val, num_rows );
        if ( x->val == NULL )
            return MAGMA_ERR_HOST_ALLOC;

        #pragma unroll
        for( magma_int_t i=0; i<num_rows; i++)
             x->val[i] = values; 
        return MAGMA_SUCCESS;  
    }
    if( memory_location == Magma_DEV ){
        x->memory_location = Magma_DEV;

        magmaDoubleComplex *tmp;

        magma_zmalloc_cpu( &tmp, num_rows );
        if ( tmp == NULL )
            return MAGMA_ERR_HOST_ALLOC;

        #pragma unroll
        for( magma_int_t i=0; i<num_rows; i++)
             tmp[i] = values; 

        if (MAGMA_SUCCESS != magma_zmalloc( &x->val, x->num_rows)) 
            return MAGMA_ERR_DEVICE_ALLOC;

        // data transfer
        magma_zsetvector( x->num_rows, tmp, 1, x->val, 1 );
 
        magma_free_cpu(tmp);

        return MAGMA_SUCCESS; 
    }
}



   


