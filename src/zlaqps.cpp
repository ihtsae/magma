/*
   -- MAGMA (version 1.1) --
   Univ. of Tennessee, Knoxville
   Univ. of California, Berkeley
   Univ. of Colorado, Denver
   November 2011

   @precisions normal z -> s d c

*/

#include "common_magma.h"
#include <cblas.h>

extern "C" magma_int_t 
magma_zlaqps(magma_int_t *m, magma_int_t *n, magma_int_t *offset, magma_int_t 
             *nb, magma_int_t *kb, cuDoubleComplex *a, magma_int_t *lda, magma_int_t *jpvt, 
             cuDoubleComplex *tau, double *vn1, double *vn2, cuDoubleComplex *auxv, cuDoubleComplex *f, 
             magma_int_t *ldf)
{
/*
    -- MAGMA (version 1.1) --
    Univ. of Tennessee, Knoxville
    Univ. of California, Berkeley
    Univ. of Colorado, Denver
    November 2011

    Purpose   
    =======   
    ZLAQPS computes a step of QR factorization with column pivoting   
    of a complex M-by-N matrix A by using Blas-3.  It tries to factorize   
    NB columns from A starting from the row OFFSET+1, and updates all   
    of the matrix with Blas-3 xGEMM.   

    In some cases, due to catastrophic cancellations, it cannot   
    factorize NB columns.  Hence, the actual number of factorized   
    columns is returned in KB.   

    Block A(1:OFFSET,1:N) is accordingly pivoted, but not factorized.   

    Arguments   
    =========   
    M       (input) INTEGER   
            The number of rows of the matrix A. M >= 0.   

    N       (input) INTEGER   
            The number of columns of the matrix A. N >= 0   

    OFFSET  (input) INTEGER   
            The number of rows of A that have been factorized in   
            previous steps.   

    NB      (input) INTEGER   
            The number of columns to factorize.   

    KB      (output) INTEGER   
            The number of columns actually factorized.   

    A       (input/output) COMPLEX*16 array, dimension (LDA,N)   
            On entry, the M-by-N matrix A.   
            On exit, block A(OFFSET+1:M,1:KB) is the triangular   
            factor obtained and block A(1:OFFSET,1:N) has been   
            accordingly pivoted, but no factorized.   
            The rest of the matrix, block A(OFFSET+1:M,KB+1:N) has   
            been updated.   

    LDA     (input) INTEGER   
            The leading dimension of the array A. LDA >= max(1,M).   

    JPVT    (input/output) INTEGER array, dimension (N)   
            JPVT(I) = K <==> Column K of the full matrix A has been   
            permuted into position I in AP.   

    TAU     (output) COMPLEX*16 array, dimension (KB)   
            The scalar factors of the elementary reflectors.   

    VN1     (input/output) DOUBLE PRECISION array, dimension (N)   
            The vector with the partial column norms.   

    VN2     (input/output) DOUBLE PRECISION array, dimension (N)   
            The vector with the exact column norms.   

    AUXV    (input/output) COMPLEX*16 array, dimension (NB)   
            Auxiliar vector.   

    F       (input/output) COMPLEX*16 array, dimension (LDF,NB)   
            Matrix F' = L*Y'*A.   

    LDF     (input) INTEGER   
            The leading dimension of the array F. LDF >= max(1,N).   

    Further Details   
    ===============   

    Based on contributions by   
      G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain   
      X. Sun, Computer Science Dept., Duke University, USA   
    =====================================================================    */
    
    static cuDoubleComplex c_b1 = MAGMA_Z_MAKE(0.,0.);
    static cuDoubleComplex c_b2 = MAGMA_Z_MAKE(1.,0.);
    static magma_int_t c__1 = 1;
    
    magma_int_t a_dim1, a_offset, f_dim1, f_offset, i__1, i__2, i__3;
    double d__1, d__2;
    cuDoubleComplex z__1;
    
    //void d_cnjg(cuDoubleComplex *, cuDoubleComplex *);
    //double z_abs(cuDoubleComplex *);
    //magma_int_t i_dnnt(double *);
    static magma_int_t j, k, rk;
    static cuDoubleComplex akk;
    static magma_int_t pvt;
    static double temp, temp2, tol3z;
    static magma_int_t itemp;

    static magma_int_t lsticc;
    static magma_int_t lastrk;    
    
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --jpvt;
    --tau;
    --vn1;
    --vn2;
    --auxv;
    f_dim1 = *ldf;
    f_offset = 1 + f_dim1;
    f -= f_offset;

    i__1 = *m, i__2 = *n + *offset;
    lastrk = min(i__1,i__2);
    lsticc = 0;
    k = 0;
    tol3z = magma_dsqrt(dlamch_("Epsilon"));
    
 L10:
    if (k < *nb && lsticc == 0) {
        ++k;
        rk = *offset + k;
        
        /* Determine ith pivot column and swap if necessary */
        i__1 = *n - k + 1;

        //pvt = k - 1 + idamax_(&i__1, &vn1[k], &c__1);

        /* Comment:
           Fortran BLAS does not have to add 1
           C       BLAS must add one to cblas_idamax */
        pvt = k - 1 + cblas_idamax(i__1, &vn1[k], c__1) +1;
        if (pvt != k) {
            blasf77_zswap(m, &a[pvt * a_dim1 + 1], &c__1, &a[k * a_dim1 + 1], &c__1);
            i__1 = k - 1;
            blasf77_zswap(&i__1, &f[pvt + f_dim1], ldf, &f[k + f_dim1], ldf);
            itemp = jpvt[pvt];
            jpvt[pvt] = jpvt[k];
            jpvt[k] = itemp;
            vn1[pvt] = vn1[k];
            vn2[pvt] = vn2[k];
        }

        /*        Apply previous Householder reflectors to column K:   
                  A(RK:M,K) := A(RK:M,K) - A(RK:M,1:K-1)*F(K,1:K-1)'. */

        if (k > 1) {
           i__1 = k - 1;
           for (j = 1; j <= i__1; ++j) {
                i__2 = k + j * f_dim1;
                //d_cnjg(&z__1, &f[k + j * f_dim1]);
                double temporary = MAGMA_Z_IMAG(f[k + j * f_dim1]);
                z__1 = MAGMA_Z_MAKE(MAGMA_Z_REAL(f[k + j * f_dim1]), -temporary);
                f[i__2] = MAGMA_Z_MAKE(MAGMA_Z_REAL(z__1),  MAGMA_Z_IMAG(z__1));
           }
           i__1 = *m - rk + 1;
           i__2 = k - 1;
           z__1 = MAGMA_Z_MAKE(-1., -0.);
           zgemv_(MagmaNoTransStr, &i__1, &i__2, &z__1, &a[rk + a_dim1], lda, 
                  &f[k + f_dim1], ldf, &c_b2, &a[rk + k * a_dim1], &c__1);
           i__1 = k - 1;
           for (j = 1; j <= i__1; ++j) {
               i__2 = k + j * f_dim1;
               //d_cnjg(&z__1, &f[k + j * f_dim1]);
               double temporary = MAGMA_Z_IMAG(f[k + j * f_dim1]);
               z__1 = MAGMA_Z_MAKE(MAGMA_Z_REAL(f[k + j * f_dim1]), -temporary);
               //f[i__2].r = z__1.r, f[i__2].i = z__1.i; 
               f[i__2] = MAGMA_Z_MAKE(MAGMA_Z_REAL(z__1), MAGMA_Z_IMAG(z__1));
           }
        }
        
        /*  Generate elementary reflector H(k). */
        if (rk < *m) {
            i__1 = *m - rk + 1;
            lapackf77_zlarfg(&i__1, &a[rk + k * a_dim1], &a[rk + 1 + k * a_dim1], &
                             c__1, &tau[k]);
        } else {
            lapackf77_zlarfg(&c__1, &a[rk + k * a_dim1], &a[rk + k * a_dim1], &c__1, &
                             tau[k]);
       }

        i__1 = rk + k * a_dim1;
       akk = MAGMA_Z_MAKE(MAGMA_Z_REAL(a[i__1]), MAGMA_Z_IMAG(a[i__1]));
       i__1 = rk + k * a_dim1;
       a[i__1] = MAGMA_Z_MAKE(1., 0.);

       /*        Compute Kth column of F:   
                 Compute  F(K+1:N,K) := tau(K)*A(RK:M,K+1:N)'*A(RK:M,K). */
       if (k < *n) {
           i__1 = *m - rk + 1;
           i__2 = *n - k;
           zgemv_(MagmaConjTransStr, &i__1, &i__2, &tau[k], &a[rk + (k + 
                  1) * a_dim1], lda, &a[rk + k * a_dim1], &c__1, &c_b1, &f[
                  k + 1 + k * f_dim1], &c__1);
       }
       
       /* Padding F(1:K,K) with zeros. */
       i__1 = k;
       for (j = 1; j <= i__1; ++j) {
           i__2 = j + k * f_dim1;
           f[i__2] = MAGMA_Z_MAKE(0., 0.);
       }
       
       /*        Incremental updating of F:   
                 F(1:N,K) := F(1:N,K) - tau(K)*F(1:N,1:K-1)*A(RK:M,1:K-1)'   
                 *A(RK:M,K). */
       
       if (k > 1) {
           i__1 = *m - rk + 1;
           i__2 = k - 1;
           i__3 = k;
           double temporary1 = MAGMA_Z_REAL(tau[i__3]);
           double temporary2 = MAGMA_Z_IMAG(tau[i__3]);
           z__1 = MAGMA_Z_MAKE(-temporary1, -temporary2);
           zgemv_(MagmaConjTransStr, &i__1, &i__2, &z__1, &a[rk + a_dim1]
                  , lda, &a[rk + k * a_dim1], &c__1, &c_b1, &auxv[1], &c__1);
           
           i__1 = k - 1;
           zgemv_(MagmaNoTransStr, n, &i__1, &c_b2, &f[f_dim1 + 1], ldf, &
                  auxv[1], &c__1, &c_b2, &f[k * f_dim1 + 1], &c__1);
       }
       
       /*        Update the current row of A:   
                 A(RK,K+1:N) := A(RK,K+1:N) - A(RK,1:K)*F(K+1:N,1:K)'. */
       
       if (k < *n) {
           i__1 = *n - k;
           z__1 = MAGMA_Z_MAKE(-1., -0.);

           blasf77_zgemm(MagmaNoTransStr, MagmaConjTransStr, &c__1, &i__1, &k, &
                  z__1, &a[rk + a_dim1], lda, &f[k + 1 + f_dim1], ldf, &
                  c_b2, &a[rk + (k + 1) * a_dim1], lda);
       }
       
       /*        Update partial column norms. */
       if (rk < lastrk) {
           i__1 = *n;
           for (j = k + 1; j <= i__1; ++j) {
               if (vn1[j] != 0.) {
                   
                   /*                 NOTE: The following 4 lines follow from the analysis in   
                                      Lapack Working Note 176. */
                   //temp = z_abs(&a[rk + j * a_dim1]) / vn1[j];

                   double real,imag,temporary3;
                   real = MAGMA_Z_REAL(a[rk + j * a_dim1]);
                   imag = MAGMA_Z_IMAG(a[rk + j * a_dim1]);

                   if(real < 0)
                       real = -real;
                   if(imag < 0)
                       imag = -imag;
                   if(imag > real){
                       temporary3 = real;
                       real = imag;
                       imag = temporary3;
                   }
                   if((real+imag) == real) {
                       temp = real;
                   } else {
                       
                       temporary3 = imag/real;
                       temporary3 = real * magma_dsqrt(1.0 + temporary3*temporary3);  /*overflow!!*/
                       temp = temporary3;
                   }
                   temp = temp / vn1[j];
                   
                   /* Computing MAX */
                   d__1 = 0., d__2 = (temp + 1.) * (1. - temp);
                   temp = max(d__1,d__2);
                   /* Computing 2nd power */
                   d__1 = vn1[j] / vn2[j];
                   temp2 = temp * (d__1 * d__1);
                   if (temp2 <= tol3z) {
                       vn2[j] = (double) lsticc;
                       lsticc = j;
                   } else {
                       vn1[j] *= magma_dsqrt(temp);
                   }
               }
           }
       }
       
       i__1 = rk + k * a_dim1;
       a[i__1] = MAGMA_Z_MAKE(MAGMA_Z_REAL(akk), MAGMA_Z_IMAG(akk));
       
       /* End of while loop. */
       goto L10;
    }
    *kb = k;
    rk = *offset + *kb;

    /*     Apply the block reflector to the rest of the matrix:   
           A(OFFSET+KB+1:M,KB+1:N) := A(OFFSET+KB+1:M,KB+1:N) -   
           A(OFFSET+KB+1:M,1:KB)*F(KB+1:N,1:KB)'.                     */
    i__1 = *n, i__2 = *m - *offset;
    if (*kb < min(i__1,i__2)) {
       i__1 = *m - rk;
       i__2 = *n - *kb;
       z__1 = MAGMA_Z_MAKE(-1., -0.);
      
       blasf77_zgemm(MagmaNoTransStr, MagmaConjTransStr, &i__1, &i__2, kb, &z__1, 
              &a[rk + 1 + a_dim1], lda, &f[*kb + 1 + f_dim1], ldf, &c_b2, &
              a[rk + 1 + (*kb + 1) * a_dim1], lda);
    }
    
    /*     Recomputation of difficult columns. */
 L60:
    if (lsticc > 0) {
       //itemp = i_dnnt(&vn2[lsticc]);
       itemp = (magma_int_t)(vn2[lsticc] >= 0. ? floor(vn2[lsticc] + .5) : -floor(.5 - vn2[lsticc]));  
       i__1 = *m - rk;
       vn1[lsticc] = cblas_dznrm2(i__1, &a[rk + 1 + lsticc * a_dim1], c__1);
       
       /*        NOTE: The computation of VN1( LSTICC ) relies on the fact that   
                 SNRM2 does not fail on vectors with norm below the value of   
                 SQRT(DLAMCH('S')) */       
       vn2[lsticc] = vn1[lsticc];
       lsticc = itemp;
       goto L60;
    }

    return 0;
} /* magma_zlaqps */
