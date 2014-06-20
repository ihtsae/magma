/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       @date

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


/**
    Purpose
    -------

    Provides for a matrix A, a blocksize, an offset, 
    and a number of matrix powers s, the 
    local matrix B to needed by this processor for the matrix power kernel.

    Arguments
    ---------

    magma_sparse_matrix A                input matrix A
    magma_sparse_matrix *B               output matrix B
    @param
    offset      magma_int_t
                first row to compute

    @param
    blocksize   magma_int_t
                number of rows to compute

    @param
    s           magma_int_t
                matrix powers


    @ingroup magmasparse_zaux
    ********************************************************************/

magma_int_t 
magma_z_mpksetup_one(  magma_z_sparse_matrix A, 
                       magma_z_sparse_matrix *B, 
                       magma_int_t offset, 
                       magma_int_t blocksize, 
                       magma_int_t s ){

    if( A.memory_location == Magma_CPU ){
        if( A.storage_type == Magma_CSR ){
            B->num_rows = A.num_rows;
            B->num_cols = A.num_cols;
            B->storage_type = A.storage_type;
            B->memory_location = A.memory_location;
            magma_index_malloc_cpu( &B->row, A.num_rows+1 );
                
            magma_int_t start = offset;
            magma_int_t end = offset + blocksize;
            //printf("local block size: %d offset: %d\n", blocksize, offset);

            magma_int_t *z1, *z2, i, j, count, num_new_nnz;
            magma_index_malloc_cpu( &z1, A.num_rows );
            magma_index_malloc_cpu( &z2, A.num_rows );
            for( i=0; i<A.num_rows; i++ )
                    z1[i] = 0;
            for( i=0; i<A.num_rows; i++ )
                    z2[i] = 0;
            for( i=start; i<end; i++ )
                    z1[i] = 1;

            // determine the rows of A needed in local matrix B 
            for( count=0; count<s-1; count++ ){
                for( j=0; j<A.num_rows; j++ ){
                    if ( z1[j] == 1 ){
                        for( i=A.row[j]; i<A.row[j+1]; i++ )
                            z2[A.col[i]] = 1;
                    }
                }
                for( i=0; i<A.num_rows; i++){
                    if( z1[i] == 1 || z2[i] ==1 )
                        z1[i] = 1; 
                }  
            }   
            // fill row pointer of B
            num_new_nnz = 0;
            for( i=0; i<A.num_rows; i++){
                B->row[i] = num_new_nnz;
                if( z1[i] != 0 ){
                    num_new_nnz += A.row[i+1]-A.row[i];
                }            
            }
            B->row[B->num_rows] = num_new_nnz;
            B->nnz = num_new_nnz;
            magma_zmalloc_cpu( &B->val, num_new_nnz );
            magma_index_malloc_cpu( &B->col, num_new_nnz );
            // fill val and col pointer of B
            num_new_nnz = 0;
            for( j=0; j<A.num_rows; j++){
                if( z1[j] != 0 ){
                    for( i=A.row[j]; i<A.row[j+1]; i++ ){
                        B->col[num_new_nnz] = A.col[i];
                        B->val[num_new_nnz] = A.val[i];
                        num_new_nnz++;
                    }
                }
            }
            magma_free_cpu(z1);
            magma_free_cpu(z2);
        }
        else{
            magma_z_sparse_matrix C, D;
            magma_z_mconvert( A, &C, A.storage_type, Magma_CSR );
            magma_z_mpksetup_one(  C, &D, offset, blocksize, s );
            magma_z_mconvert( D, B, Magma_CSR, A.storage_type );
            magma_z_mfree(&C);
            magma_z_mfree(&D);
        }
    }
    else{
        magma_z_sparse_matrix C, D;
        magma_z_mtransfer( A, &C, A.memory_location, Magma_CPU );
        magma_z_mpksetup_one(  C, &D, offset, blocksize, s );
        magma_z_mtransfer( D, B, Magma_CPU, A.memory_location );
        magma_z_mfree(&C);
        magma_z_mfree(&D);
    }
     

    return MAGMA_SUCCESS; 
}


/**
    Purpose
    -------

    Provides for a matrix A, a number of processors num_procs, 
    and a distribution *offset, *blocksize and s a set of matrices B
    each containing the matrix rows to compute the matrix power kernel.

    Arguments
    ---------

    magma_sparse_matrix A                input matrix A
    magma_sparse_matrix B[MagmaMaxGPUs]  set of output matrices B
    @param
    num_procs   magma_int_t
                number of processors

    @param
    offset      magma_int_t*
                array containing the offsets

    @param
    blocksize   magma_int_t*
                array containing the chunk sizes

    @param
    s           magma_int_t
                matrix powers


    @ingroup magmasparse_zaux
    ********************************************************************/

magma_int_t 
magma_z_mpksetup(  magma_z_sparse_matrix A, 
                   magma_z_sparse_matrix B[MagmaMaxGPUs], 
                   magma_int_t num_procs,
                   magma_int_t *offset, 
                   magma_int_t *blocksize, 
                   magma_int_t s ){

    for(int procs=0; procs<num_procs; procs++){
            magma_z_mpksetup_one( A, &B[procs], offset[procs], 
                                            blocksize[procs], s );        
    }

    return MAGMA_SUCCESS; 
}




/**
    Purpose
    -------

    Provides for a matrix A, a blocksize, an offset, 
    and a number of matrix powers s, the number of additional rows 
    that are needed by this processor for the matrix power kernel.

    Arguments
    ---------

    magma_sparse_matrix A                input matrix A
    @param
    offset      magma_int_t
                first row to compute

    @param
    blocksize   magma_int_t
                number of rows to compute

    @param
    s           magma_int_t
                matrix powers

    @param
    num_add_rowsmagma_int_t**
                number of additional rows for each s

    @param
    add_rows    magma_int_t*
                array for additional rows ordered 
    \n
                                                                according to s
    @param
    num_add_vecsmagma_int_t*
                number of additional vector entries

    @param
    add_vecs    magma_int_t*
                array for additional vector entries


    @ingroup magmasparse_z
    ********************************************************************/

magma_int_t 
magma_z_mpkinfo_one( magma_z_sparse_matrix A, 
                     magma_int_t offset, 
                     magma_int_t blocksize, 
                     magma_int_t s,    
                     magma_index_t **num_add_rows,
                     magma_index_t **add_rows,
                     magma_int_t *num_add_vecs,
                     magma_index_t **add_vecs ){

    if( A.memory_location == Magma_CPU ){
        if( A.storage_type == Magma_CSR ){
            magma_int_t start = offset;
            magma_int_t end = offset + blocksize;

            magma_int_t *z1, *z2, i, j, count;
            magma_index_malloc_cpu( &z1, A.num_rows );
            magma_index_malloc_cpu( &z2, A.num_rows );
            for( i=0; i<A.num_rows; i++ )
                    z1[i] = 0;
            for( i=0; i<A.num_rows; i++ )
                    z2[i] = 0;
            for( i=start; i<end; i++ )
                    z1[i] = 1;

            magma_index_malloc_cpu( (num_add_rows), s );
            (*num_add_rows)[0] = 0;

            // determine the rows of A needed in local matrix B 
            for( count=1; count<s; count++ ){
                for( j=0; j<A.num_rows; j++ ){
                    if ( z1[j] != 0 ){
                        for( i=A.row[j]; i<A.row[j+1]; i++ )
                            z2[A.col[i]] = count;
                    }
                }
                for( i=0; i<A.num_rows; i++){
                    if(  z2[i] != 0 )
                        z1[i] = z2[i]; 
                }  

                (*num_add_rows)[count] = 0;
                for( i=0; i<start; i++){
                    if( z1[i] != 0 )
                        (*num_add_rows)[count]++;
                }
                for( i=end; i<A.num_rows; i++){
                    if( z1[i] != 0 )
                        (*num_add_rows)[count]++;
                }
            }   

            // this part determines the additional rows needed
            magma_index_malloc_cpu( (add_rows), (*num_add_rows)[s-1] );
            magma_int_t num_add_rows_glob = 0;
            for( count=1; count<s; count++ ){
                for( i=0; i<start; i++){
                    if( z1[i] == count ){
                        (*add_rows)[num_add_rows_glob] = i;
                        (num_add_rows_glob)++;
                    }
                }
                for( i=end; i<A.num_rows; i++){
                    if( z1[i] == count ){
                        (*add_rows)[num_add_rows_glob] = i;
                        (num_add_rows_glob)++;

                    }
                }  
            }

            // for the vector entries, we need one more loop
            for( j=0; j<A.num_rows; j++ ){
                if ( z1[j] != 0 ){
                    for( i=A.row[j]; i<A.row[j+1]; i++ )
                        z2[A.col[i]] = 1;
                }
            }
            for( i=0; i<A.num_rows; i++){
                if( z1[i] != 0 || z2[i] != 0 )
                    z1[i] = 1; 
            }  
            (*num_add_vecs) = 0;
            for( i=0; i<start; i++){
                if( z1[i] != 0 )
                    (*num_add_vecs)++;
            }
            for( i=end; i<A.num_rows; i++){
                if( z1[i] != 0 )
                    (*num_add_vecs)++;
            }
            // this part determines the additional vector entries needed
            magma_index_malloc_cpu( (add_vecs), (*num_add_vecs) );
            (*num_add_vecs) = 0;
            for( i=0; i<start; i++){
                if( z1[i] != 0 ){
                    (*add_vecs)[(*num_add_vecs)] = i;
                    (*num_add_vecs)++;
                }
            }
            for( i=end; i<A.num_rows; i++){
                if( z1[i] != 0 ){
                    (*add_vecs)[(*num_add_vecs)] = i;
                    (*num_add_vecs)++;
                }
            }  

            magma_free_cpu(z1);
            magma_free_cpu(z2);


        }
        else{
            magma_z_sparse_matrix C;
            magma_z_mconvert( A, &C, A.storage_type, Magma_CSR );
            magma_z_mpkinfo_one(  C, offset, blocksize, s, 
                    num_add_rows, add_rows, num_add_vecs, add_vecs );
            magma_z_mfree(&C);
        }
    }
    else{
        magma_z_sparse_matrix C;
        magma_z_mtransfer( A, &C, A.memory_location, Magma_CPU );
        magma_z_mpkinfo_one(  C, offset, blocksize, s, 
                    num_add_rows, add_rows, num_add_vecs, add_vecs );
        magma_z_mfree(&C);
    }

   return MAGMA_SUCCESS; 
}


/**
    Purpose
    -------

    Extension of magma_z_mpkinfo. The idea is to have an array containing 
    for all GPU sets which data is needed. Furthermore, it 
    provides for a matrix A, a number of processors num_procs, 
    and a distribution *offset, *blocksize and s the number of 
    rows added to the different matrices.

    num_add_vecs = [ GPU0 from Host, GPU1 from GPU0, GPU2 from GPU0 .
                     GPU0 from GPU1, GPU1 from Host, GPU2 from GPU0 .
                     ... ]

    add_vecs has the same structure, but instead of the number, the
    component indices are provided.

    Arguments
    ---------

    magma_sparse_matrix A                input matrix A
    @param
    num_procs   magma_int_t
                number of processors

    @param
    offset      magma_int_t*
                array containing the offsets

    @param
    blocksize   magma_int_t*
                array containing the chunk sizes

    @param
    s           magma_int_t
                matrix powers

    @param
    num_addvecs magma_int_t*
                output array: number of additional rows
                
    @param
    num_addvecs magma_int_t*
                output array: number of additional rows

    @param
    num_addvec smagma_int_t*
                number of additional vector entries

    @param
    add_vecs    magma_int_t*
                array for additional vector entries


    @ingroup magmasparse_z
    ********************************************************************/

magma_int_t 
magma_z_mpkback(  magma_z_sparse_matrix A, 
                   magma_int_t num_procs,
                   magma_int_t *offset, 
                   magma_int_t *blocksize, 
                   magma_int_t s,
                   magma_int_t *num_addvecs,
                   magma_index_t **add_vecs,
                   magma_int_t *num_vecs_back,
                   magma_index_t **vecs_back ){

    magma_int_t i,j,k;
    magma_int_t *back_vecs_tmp;
    magma_index_malloc_cpu( &(back_vecs_tmp), A.num_rows );
    for( i=0; i<A.num_rows; i++ )
        back_vecs_tmp[i] = 0;

    // determine components to copy back
    for( i=0; i<num_procs; i++ ){
        for( j=0; j<num_add_vecs[i]; j++ ){
            back_vecs_tmp[add_vecs[i][j]] = 1;
        }
    }
    for( i=0; i<num_procs; i++ ){
        num_vecs_back[i] = 0;
        for( j=offset[i]; j<offset[i]+blocksize[i]; j++ ){
            if(  back_vecs_tmp[j] == 1 )
                num_vecs_back[i]++;
        }
        magma_index_malloc_cpu( &(vecs_back[i]), (num_vecs_back[i]) );
    }
    
    // fill in the information into vecs_back
    for( i=0; i<num_procs; i++ ){
        k=0;
        for( j=offset[i]; j<offset[i]+blocksize[i]; j++ ){
            if(  back_vecs_tmp[j] == 1 ){
                 vecs_back[i][k] = j;
                 k++;
            }
        }
    }

    return MAGMA_SUCCESS; 
}


/**
    Purpose
    -------

    Provides for a matrix A, a number of processors num_procs, 
    and a distribution *offset, *blocksize and s the number of 
    rows added to the different matrices.

    Arguments
    ---------

    magma_sparse_matrix A                input matrix A
    @param
    num_procs   magma_int_t
                number of processors

    @param
    offset      magma_int_t*
                array containing the offsets

    @param
    blocksize   magma_int_t*
                array containing the chunk sizes

    @param
    s           magma_int_t
                matrix powers

    @param
    num_addrows magma_int_t*
                output array: number of additional rows

    @param
    num_addvecs magma_int_t*
                number of additional vector entries

    @param
    add_vecs    magma_int_t*
                array for additional vector entries


    @ingroup magmasparse_zaux
    ********************************************************************/

magma_int_t 
magma_z_mpkinfo(   magma_z_sparse_matrix A, 
                   magma_int_t num_procs,
                   magma_int_t *offset, 
                   magma_int_t *blocksize, 
                   magma_int_t s,
                   magma_index_t **num_addrows,
                   magma_index_t **add_rows,
                   magma_int_t *num_addvecs,
                   magma_index_t **add_vecs,
                   magma_int_t *num_vecs_back,
                   magma_index_t **vecs_back ){

    for(int procs=0; procs<num_procs; procs++){
        magma_z_mpkinfo_one( A, offset[procs], blocksize[procs], s, 
            &num_add_rows[procs], &add_rows[procs], 
            &num_add_vecs[procs], &add_vecs[procs] ); 
    }

    magma_z_mpkback( A, num_procs, offset, blocksize, s, 
                num_add_vecs, add_vecs, num_vecs_back, vecs_back );

    return MAGMA_SUCCESS; 
}



/**
    Purpose
    -------

    Packs for a given vector x and a key add_rows a compressed version
    where the add_rows are in consecutive order.

    Arguments
    ---------

    @param
    num_addrows magma_int_t
                number of elements to pack

    @param
    add_rows    magma_int_t*
                indices of elements to pack

    @param
    x           magmaDoubleComplex*
                uncompressed input vector

    @param
    y           magmaDoubleComplex*
                compressed output vector


    @ingroup magmasparse_zaux
    ********************************************************************/

magma_int_t 
magma_z_mpk_compress(    magma_int_t num_addrows,
                         magma_index_t *add_rows,
                         magmaDoubleComplex *x,
                         magmaDoubleComplex *y ){

    for(magma_int_t i=0; i<num_add_rows; i++){
        y[i] = x[ add_rows[i] ];
    }

    return MAGMA_SUCCESS; 
}

/**
    Purpose
    -------

    Unpacks for a given vector x and a key add_rows the compressed version.

    Arguments
    ---------

    @param
    num_add_rowsmagma_int_t
                number of elements to pack

    @param
    add_rows    magma_int_t*
                indices of elements to pack

    @param
    x           magmaDoubleComplex*
                compressed input vector

    @param
    y           magmaDoubleComplex*
                uncompressed output vector


    @ingroup magmasparse_zaux
    ********************************************************************/

magma_int_t 
magma_z_mpk_uncompress(  magma_int_t num_add_rows,
                         magma_index_t *add_rows,
                         magmaDoubleComplex *x,
                         magmaDoubleComplex *y ){

    for(magma_int_t i=0; i<num_add_rows; i++){
        y[add_rows[i]] = x[ i ];
    }

    return MAGMA_SUCCESS; 
}



/**
    Purpose
    -------

    Unpacks for a given vector x and a key add_rows the compressed version.
    The selective version uncompresses only vectors within the area
    [ offset , offset+blocksize ]

    Arguments
    ---------

    @param
    num_addvecs magma_int_t
                number of elements to pack

    @param
    add_vecs    magma_int_t*
                indices of elements to pack

    @param
    offset      magma_int_t
                lower bound of vector chunk

    @param
    blocksize   magma_int_t
                number of locally computed elements

    @param
    x           magmaDoubleComplex*
                compressed input vector

    @param
    y           magmaDoubleComplex*
                uncompressed output vector


    @ingroup magmasparse_zaux
    ********************************************************************/

magma_int_t 
magma_z_mpk_uncompress_sel(     magma_int_t num_addvecs,
                                magma_index_t *add_vecs,
                                magma_int_t offset,
                                magma_int_t blocksize,
                                magmaDoubleComplex *x,
                                magmaDoubleComplex *y ){

    for(magma_int_t i=0; i<num_add_vecs; i++){
        if( ( offset <= add_vecs[i] ) && ( add_vecs[i] < offset+blocksize) )
            y[add_vecs[i]] = x[ i ];
    }

    return MAGMA_SUCCESS; 
}



/**
    Purpose
    -------

    Packs the local matrix for higher SpMV performance.

    Arguments
    ---------

    @param
    num_addrows magma_int_t
                number of elements to pack

    @param
    add_rows    magma_int_t*
                indices of elements to pack

    @param
    x           magmaDoubleComplex*
                uncompressed input vector

    @param
    y           magmaDoubleComplex*
                compressed output vector


    @ingroup magmasparse_zaux
    ********************************************************************/

magma_int_t
magma_z_mpk_mcompresso(      magma_z_sparse_matrix A,
                             magma_z_sparse_matrix *B,
                             magma_int_t offset,
                             magma_int_t blocksize,
                             magma_int_t num_add_rows,
                             magma_index_t *add_rows ){
    if( A.storage_type==Magma_CSR && A.memory_location==Magma_CPU ){
            int i=0,j,n=0,nnz=0;
            B->storage_type = Magma_CSR;
            B->memory_location = A.memory_location;
            B->num_rows = blocksize + num_add_rows;
            B->num_cols = A.num_cols;
            magma_index_malloc_cpu( &B->row, B->num_rows+1 );
            for( j=0; j<blocksize+1; j++ ){
                (B->row)[n] = A.row[j+offset] - A.row[offset];
                i = (B->row)[j];
                n++;
            }


            for( j=0; j<num_add_rows; j++ ){
                i += A.row[add_rows[j]+1] - A.row[add_rows[j]];
                (B->row)[n] = i;
                n++;
            }
            B->nnz = i;
            magma_index_malloc_cpu( &B->col, B->nnz );
            magma_zmalloc_cpu( &B->val, B->nnz );

            for( j=A.row[offset]; j<A.row[offset+blocksize]; j++ ){
                B->col[nnz] = A.col[j];
                B->val[nnz] = A.val[j];
                nnz++;
            }

            for( j=0; j<num_add_rows; j++ ){
                for( i=A.row[add_rows[j]]; i<A.row[add_rows[j]+1]; i++){
                    B->col[nnz] = A.col[i];
                    B->val[nnz] = A.val[i];
                    nnz++;
                }
            }
            // printf("\n");magma_z_mvisu(*B);printf("\n");
    }
    else{
        printf("not supported!\n");
    }

    return MAGMA_SUCCESS; 
}




