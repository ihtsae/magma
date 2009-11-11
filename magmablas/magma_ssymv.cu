#include <stdio.h>
#include "cuda.h"
#define BLOCK_SIZE 32

//#define num_threads 64
#define dgemv_bs 32




__global__ void
l_ssymv_special (int n, float alpha ,  float* A, int lda, float *x, int ldx , float beta ,  float *y , int ldy ){
  int tx = threadIdx.x ; 
  int ty = threadIdx.y ; 
  int ind = blockIdx.x*  dgemv_bs + tx ;
  float res = 0.f;


  __shared__ float buff [dgemv_bs];
  __shared__ float la   [dgemv_bs][dgemv_bs+1];


  A += ind;
  x += tx ;
  A+= ty * lda  ;  
  int break_d  =   blockIdx.x* dgemv_bs ;

  for(int  i=0; i<break_d; i += dgemv_bs ){
    #pragma unroll 8 
    for(int j=0; j < dgemv_bs ; j+=4){
        la[tx][ty+j] = A[j*lda] ;
    }
    buff[tx]  = x[i];
    __syncthreads();

    #pragma unroll 8 
    for(int j=0; j < 8 ; j++){
       res+=la[tx][j+ty*8]*buff[j+ty*8];
    }
    A+=lda* dgemv_bs ;
    __syncthreads(); 
  }

 

  #pragma unroll 8
  for(int j =0; j<dgemv_bs; j+=4)
         la[ty+j][tx] = A[ j * lda];


  A+= dgemv_bs ;
  x+= break_d ; 
  __syncthreads();
  //buff[tx]  = x[break_d];
  buff[tx]  = x[0];
  #pragma unroll 8
  for(int  i=ty*8; i<(1+ty)* dgemv_bs/4 ; i++){
         if ( i < tx )   {
	        la[tx][i] = la[i][tx] ; 
         }
	 else 
	        la[tx][i] = la[tx][i]  ;
  
  }
  __syncthreads();
    #pragma unroll 8
    for(int j=0; j < dgemv_bs/4 ; j++){
     res+=la[tx][j+ty*8]*buff[j+ty*8];
    }
  x-= break_d ; 
  break_d  += dgemv_bs ; 
  __syncthreads();




  for(int i=break_d; i<n; i += dgemv_bs ){
    buff[tx]  = x[i];
   #pragma unroll 8
    for(int j=0; j<dgemv_bs; j+=4)
       la[ty+j][tx] = A[ j * lda];
    A+= dgemv_bs ;
      __syncthreads();
    #pragma unroll 8
    for(int j=0; j < dgemv_bs/4;j++){
       res+=la[tx][j+ty*8]*buff[j+ty*8];
    }
      __syncthreads();
  }


   la[tx][ty]= res ;
   __syncthreads();
   if( ty == 0 ) {
     res = res + la[tx][1]+ la[tx][2]+ la[tx][3] ;
     y[ind] = beta * y[ind]  + alpha * res;
   }

}

__global__ void
l_ssymv_generic (int n, float alpha ,  float* A, int lda, float *x, int ldx , float beta ,  float *y , int ldy , int m_full_block , int m_mod_32){

  
  int tx = threadIdx.x ; 
  int ty = threadIdx.y ; 

  int ind = blockIdx.x*  dgemv_bs + tx ;
  
  float res = 0.f;


  __shared__ float buff [dgemv_bs];
  __shared__ float la   [dgemv_bs][dgemv_bs+1];

  if( blockIdx.x == m_full_block ) {
  /************************************************************************
   -- Last block --
   -- We will do something unusual here 
   -- For sufficiently large matrix the overhead will be very low
  *************************************************************************/
       if  ( tx < m_mod_32 ){
		A+= ( blockIdx.x * dgemv_bs + tx ) ;
       } 	 	
       else{
		A+= ( blockIdx.x * dgemv_bs + m_mod_32 -1) ; 
		//A+= ( blockIdx.x * dgemv_bs + 0) ; 
       }
       x+=tx;
       A+= ty * lda  ;  
       int break_d  =   blockIdx.x* dgemv_bs ;

	  /*----------------------------
		Go Right
	  -------------------------------*/

	  for(int  i=0; i<break_d; i += dgemv_bs ){
	    #pragma unroll 8 
	    for(int j=0; j < dgemv_bs ; j+=4){
	        la[tx][ty+j] = A[j*lda] ;
	    }
	    buff[tx]  = x[i];
	    __syncthreads();

	    #pragma unroll 8 
	    for(int j=0; j < 8 ; j++){
	       res+=la[tx][j+ty*8]*buff[j+ty*8];
	    }
	    A+=lda* dgemv_bs ;
	    __syncthreads(); 
	  }
	  /*
           we don't need to make zero, as those computation will be discarded. 
          */
          if( ty==0  ) {
		x+= ( break_d -tx ) ; 
	        //buff[tx]  = x[i];
		/*--------------------------------------------
			he will compute the triangular parts
			others will be waiting with values. 
                -----------------------------------------------*/
		int j ;
                int count = 1 ; 
		if( tx < m_mod_32 ) 
			count = tx ; 
		else
			count = m_mod_32 ;
		for(j =0;j<=count;j++){
			res+= A[j*lda] * x[j];
                }
		A+=(tx)*lda;
		count = 1 ; 
		for(;j<m_mod_32;j++){
			res+= A[count] * x[j];
			count++;
		}
          }
          else{
          }
	  __syncthreads(); 
   	 la[tx][ty]= res ;
          __syncthreads();
         /*--------------------------------------------------------
	 The leader accumulates all the results from his peer. 
         ----------------------------------------------------------*/
         if( ty == 0 ) {
             res = res + la[tx][1]+ la[tx][2]+ la[tx][3] ;
	     if( tx < m_mod_32)
                 y[ind] = beta * y[ind]  + alpha * res;
         }
	 
  }

  else{ 
  /***************************************
    -----------------------------------
  -- All the blocks but the last one --
  ****************************************
  -------------------------------------*/
  A += ind;
  x += tx ;
  A+= ty * lda  ;  
  int break_d  =   blockIdx.x* dgemv_bs ;

  /*----------------------------
	Go Right
  -------------------------------*/
  for(int  i=0; i<break_d; i += dgemv_bs ){
    #pragma unroll 8 
    for(int j=0; j < dgemv_bs ; j+=4){
        la[tx][ty+j] = A[j*lda] ;
    }
    buff[tx]  = x[i];
    __syncthreads();

    #pragma unroll 8 
    for(int j=0; j < 8 ; j++){
       res+=la[tx][j+ty*8]*buff[j+ty*8];
    }
    A+=lda* dgemv_bs ;
    __syncthreads(); 
  }

 
  /*------------------------------------
	Diagonal 
	Copy + Transpose lower triangle
  --------------------------------------*/
  #pragma unroll 8
  for(int j =0; j<dgemv_bs; j+=4)
         la[ty+j][tx] = A[ j * lda];


  A+= dgemv_bs ;
  x+= break_d ; 
  __syncthreads();
  //buff[tx]  = x[break_d];
  buff[tx]  = x[0];
  /*--------------------------------------------
	Mirror Upper Triangle to Lower triangle
  ---------------------------------------------*/
  #pragma unroll 8
  for(int  i=ty*8; i<(1+ty)* dgemv_bs/4 ; i++){
         if ( i < tx )   {
	        la[tx][i] = la[i][tx] ; 
         }
	 else 
	        la[tx][i] = la[tx][i]  ;
  
  }
  __syncthreads();
  /*--------------------------------
	Do diagonal Computation
  -----------------------------------*/
    #pragma unroll 8
    for(int j=0; j < dgemv_bs/4 ; j++){
     res+=la[tx][j+ty*8]*buff[j+ty*8];
    }
  x-= break_d ; 
  break_d  += dgemv_bs ; 
  __syncthreads();


  n -= m_mod_32 ;  // @ 
  /*-----------------------------
	Go Down 
  -------------------------------*/
  for(int i=break_d; i<n; i += dgemv_bs ){
    buff[tx]  = x[i];
   #pragma unroll 8
    for(int j=0; j<dgemv_bs; j+=4)
       la[ty+j][tx] = A[ j * lda];
    A+= dgemv_bs ;
      __syncthreads();
    #pragma unroll 8
    for(int j=0; j < dgemv_bs/4;j++){
       res+=la[tx][j+ty*8]*buff[j+ty*8];
    }
      __syncthreads();
  }

  
  /*---------------------------------------------
	doing m_mod_32 stuffs here.
	Symmetric is giving us benefit .. true
  -----------------------------------------------*/
    x-=tx;
    x+=n;
    A-=tx;
    if( tx < m_mod_32){
        buff[tx]  = x[tx];
	A+=tx;
    }
    else{
        buff[tx]  = 0*x[m_mod_32-1]; /*This will confirm valid memory reference*/
	A+=(m_mod_32-1); /* Same as above*/
    }

   #pragma unroll 8
    for(int j=0; j<dgemv_bs; j+=4){
       if( tx < m_mod_32 ) 
       la[ty+j][tx] = 1.0 * A[ j * lda];
       else
       la[ty+j][tx] = 0.0 * A[ j * lda];
       
    }
    __syncthreads();

    /*----------------------------------------
	What about doing some Zeroing here?
	instead of zeroing before?
    -----------------------------------------*/	
    #pragma unroll 8
    for(int j=0; j < dgemv_bs/4;j++){
       //if( ( j+ty*8) < m_mod_32 )
       res+=la[tx][j+ty*8]*buff[j+ty*8];
    }
    __syncthreads();
   

   la[tx][ty]= res ;
   __syncthreads();
   /*--------------------------------------------------------
	The leader accumulates all the results from his peer. 
   ----------------------------------------------------------*/
   if( ty == 0 ) {
     res = res + la[tx][1]+ la[tx][2]+ la[tx][3] ;
     y[ind] = beta * y[ind]  + alpha * res;
   }

  }

}

__global__ void
u_ssymv_generic (int n, float alpha ,  float* A, int lda, float *x, int ldx , float beta ,  float *y , int ldy , int m_full_block , int m_mod_32){

  
  int tx = threadIdx.x ; 
  int ty = threadIdx.y ; 

  int ind = blockIdx.x*  dgemv_bs + tx ;
  
  float res = 0.f;


  __shared__ float buff [dgemv_bs];
  __shared__ float la   [dgemv_bs][dgemv_bs+1];
  int blockIdxx =  blockIdx.x ;

  if( blockIdx.x == m_full_block ) {

  /************************************************************************
   -- Last block --
   -- We will do something unusual here 
   -- For sufficiently large matrix the overhead will be very low
  *************************************************************************/

  ind =  tx ;
  A+= lda*(n-1) ; 
  x+= (n-1);


       if  ( tx < m_mod_32 ){
		A+= (  tx ) ;
       } 	 	
       else{
		A+= (  m_mod_32 -1) ; 
       }
       x-=tx;
       A-= ty * lda  ;  
       int break_d  =   (blockIdx.x)* dgemv_bs ;

	  /*----------------------------
		Go Right
	  -------------------------------*/

	  for(int  i=0; i<break_d; i += dgemv_bs ){
	    #pragma unroll 8 
	    for(int j=0; j < dgemv_bs ; j+=4){
	        la[tx][ty+j] = A[-j*lda] ;
	    }
	    buff[tx]  = x[-i];
	    __syncthreads();

	    #pragma unroll 8 
	    for(int j=0; j < 8 ; j++){
	       res+=la[tx][j+ty*8]*buff[j+ty*8];
	    }
	    A-=lda* dgemv_bs ;
	    __syncthreads(); 
	  }
	  /*
           we don't need to make zero, as those computation will be discarded. 
          */
          if( ty==0  ) {
		x-= ( break_d -tx ) ; 
	        //buff[tx]  = x[i];
		/*--------------------------------------------
			he will compute the triangular parts
			others will be waiting with values. 
                -----------------------------------------------*/
		int j ;
                int count = 1 ; 
		if( tx < m_mod_32 ) 
			count =m_mod_32- tx ; 
		else
			count = m_mod_32 ;
		for(j =0;j<count;j++){
			res+= A[-j*lda] * x[-j];
                }
		A-=(count-1)*lda;
		count = 1 ; 
		for(;j<m_mod_32;j++){
			res+= A[-count] * x[-j];
			count++;
		}
          }
          else{
          }
	  __syncthreads(); 
   	 la[tx][ty]= res ;
          __syncthreads();
         /*--------------------------------------------------------
	 The leader accumulates all the results from his peer. 
         ----------------------------------------------------------*/
         if( ty == 0 ) {
             res = res + la[tx][1]+ la[tx][2]+ la[tx][3] ;
	     if( tx < m_mod_32)
                 y[ind] = beta * y[ind]  + alpha * res;
         }
	 
  }

  else{ 
  /***************************************
    -----------------------------------
  -- All the blocks but the last one --
  -- By the way this code can be optimized more. 
  ****************************************
  -------------------------------------*/
  ind = blockIdx.x *  dgemv_bs + tx + m_mod_32 ;
  float *A1 = A ; 
  float *x1 = x ; 
  A+= lda*(n-1)  ; 
  x+= (n-1);

  A += ind;
  x -= tx ;
  A-= ty * lda  ;  

  int break_d  = (n / dgemv_bs -   blockIdxx-1 )* dgemv_bs ;
  /*----------------------------
	Go Left
  -------------------------------*/
  for(int  i=0; i<break_d; i += dgemv_bs ){
    #pragma unroll 8 
    for(int j=0; j < dgemv_bs ; j+=4){
        la[tx][ty+j] = A[-j*lda] ;
    }
    buff[tx]  = x[-i];
    __syncthreads();

    #pragma unroll 8 
    for(int j=0; j < 8 ; j++){
       res+=la[tx][j+ty*8]*buff[j+ty*8];
    }
    A-=lda* dgemv_bs ;
    __syncthreads(); 
  }

 
  /*------------------------------------
	Diagonal 
	Copy + Transpose lower triangle
  --------------------------------------*/
  #pragma unroll 8
  for(int j =0; j<dgemv_bs; j+=4){
         la[tx][31-ty-j] = A[ -j * lda];
  }

  A-= dgemv_bs ;
  x-= break_d ; 
  __syncthreads();
  buff[31-tx]  = x[0];
  /*--------------------------------------------
	Mirror Upper Triangle to Lower triangle
  ---------------------------------------------*/
  #pragma unroll 8
  for(int  i=ty*8; i<(1+ty)* dgemv_bs/4 ; i++){
         if ( i <tx ){
	        la[tx][i] = la[i][tx]; 
         }
	 else{ 
	        la[tx][i] = la[tx][i]  ;
	 }
  }
  __syncthreads();
  /*--------------------------------
	Do diagonal Computation
  -----------------------------------*/
    #pragma unroll 8
    for(int j=0; j < dgemv_bs/4 ; j++){
     res+=la[tx][j+ty*8]*buff[j+ty*8];
    }
  x+=break_d  ; 
  break_d  += dgemv_bs ; 
  __syncthreads();


  n -= m_mod_32 ;  // @ 
  /*-----------------------------
	Go Up 
  -------------------------------*/
  int i ;
  for( i=break_d; i<n; i+= dgemv_bs ){
    buff[31-tx]  = x[-i] ;
   #pragma unroll 8
    for(int j=0; j<dgemv_bs; j+=4){
       la[ty+j][tx] = A[- j * lda];
    }
    A-= dgemv_bs ;
      __syncthreads();
    #pragma unroll 8
    for(int j=0; j < dgemv_bs/4;j++){
       res+=la[31-tx][j+ty*8]*buff[j+ty*8];
    }
      __syncthreads();
  }
  /*---------------------------------------------
	doing m_mod_32 stuffs here.
	Symmetric is giving us benefit .. true
	Do the other way please......
  -----------------------------------------------*/
   A1 = A1 + m_mod_32 * lda + tx *lda ;  
   if( ty == 0  ) {
	for( int j = 0 ;  j < m_mod_32 ; j++){
		res+= x1[j] * A1[ j + lda * (blockIdx.x) * 32 ];
	}
   }
    __syncthreads();

   la[tx][ty]= res ;
   __syncthreads();
   /*--------------------------------------------------------
	The leader accumulates all the results from his peer. 
   ----------------------------------------------------------*/
   if( ty == 0 ) {
     res = res + la[tx][1]+ la[tx][2]+ la[tx][3] ;
     y[ind] = beta * y[ind]  + alpha * res;
   }

  }

}






__global__ void
u_ssymv_special (int n, float alpha ,  float* A, int lda, float *x, int ldx , float beta ,  float *y , int ldy ){
  int tx = threadIdx.x ; 
  int ty = threadIdx.y ; 
  int ind = blockIdx.x*  dgemv_bs + tx ;
  float res = 0.f;

  /*
	Reverse Computation ... 
		- Left 
		- Triangle 
		- Up 
  */

  A+= lda*(n-1) ; 
  x+= (n-1);
  __shared__ float buff [dgemv_bs];
  __shared__ float la   [dgemv_bs][dgemv_bs+1];


  A += ind;
  x -= tx ;
  A-= ty * lda  ;  
  int break_d  = (n / dgemv_bs -   blockIdx.x-1 )* dgemv_bs ;

  for(int  i=0; i<break_d; i += dgemv_bs ){
    #pragma unroll 8 
    for(int j=0; j < dgemv_bs ; j+=4){
        la[tx][ty+j] = A[-j*lda] ;
    }
    buff[tx]  = x[-i];
    __syncthreads();

    #pragma unroll 8 
    for(int j=0; j < 8 ; j++){
       res+=la[tx][j+ty*8]*buff[j+ty*8];
    }
    A-=lda* dgemv_bs ;
    __syncthreads(); 
  }




  #pragma unroll 8
  for(int j =0; j<dgemv_bs; j+=4)
         la[tx][31-ty-j] = A[ -j * lda];
  /*
	Look at the indexing changes
  */

  A-= dgemv_bs ;
  x-= break_d ; 
  __syncthreads();
  buff[31-tx]  = x[0];
  #pragma unroll 8
  for(int  i=ty*8; i<(1+ty)* dgemv_bs/4 ; i++){
         if ( i <tx ){
	        la[tx][i] = la[i][tx]; 
         }
	 else{ 
	        la[tx][i] = la[tx][i]  ;
	 }
  
  }
  __syncthreads();
    #pragma unroll 8
    for(int j=0; j < dgemv_bs/4 ; j++){
     res+=la[tx][j+ty*8]*buff[j+ty*8];
    }

 x+=break_d  ; 
  break_d  += dgemv_bs ; 
  __syncthreads();



  for(int i=break_d; i<n; i+= dgemv_bs ){
    buff[31-tx]  = x[-i] ;
   #pragma unroll 8
    for(int j=0; j<dgemv_bs; j+=4)
       la[ty+j][tx] = A[ -j * lda];

    A-= dgemv_bs ;
      __syncthreads();
    #pragma unroll 8
    for(int j=0; j < dgemv_bs/4;j++){
       res+=la[31-tx][j+ty*8]*buff[j+ty*8];
    }
      __syncthreads();
  }


   la[tx][ty]= res ;

   __syncthreads();
   if( ty == 0 ) {
     res = res + la[tx][1]+ la[tx][2]+ la[tx][3] ;
     y[ind] =  beta * y[ind]  + alpha * res;
   }

}





extern "C" void mssymv (char side , char uplo , int m , float alpha ,  float *A , int lda , 
 float *X , int incx , float beta , float *Y , int incy )
{
/*
Note:
	The UPLO = 'U' Version can be optimized more.
        side is not needed........................... 
*/
    int blocks;
    if (m % dgemv_bs==0)
        blocks = m/ dgemv_bs;
    else
        blocks = m/ dgemv_bs + 1;

    dim3 grid(blocks, 1, 1);
    dim3 threads(32, 4, 1);

    if( m % dgemv_bs == 0 ) {
	    if( uplo == 'L' || uplo == 'l'){	
		    l_ssymv_special <<<grid, threads>>> (m, alpha , A, lda, X, incx ,beta,  Y , incy);
	    }
            else{
		    u_ssymv_special <<<grid, threads>>> (m, alpha , A, lda, X, incx ,beta,  Y , incy);
	    } 
		
    } 
    else{	
	    int  m_full_block = (m - m % 32 ) /32 ; 
	    int  m_mod_32 = m%32 ;  
	    if( uplo == 'L' || uplo == 'l'){
		    l_ssymv_generic <<<grid, threads>>> (m, alpha , A, lda, X, incx ,beta,  Y , incy, m_full_block , m_mod_32);
	    }	
	    else{
		    u_ssymv_generic <<<grid, threads>>> (m, alpha , A, lda, X, incx ,beta,  Y , incy, m_full_block , m_mod_32);
	    }	
    }
}


/*
Interface ..................................
*/

extern "C" void 
magma_ssymv (char side , char uplo , int m , float alpha ,  float *A , int lda ,  float *X , int incx , float beta , float *Y , int incy )
{
	mssymv (side, uplo , m , alpha , A , lda , X , incx , beta , Y , incy );
}
