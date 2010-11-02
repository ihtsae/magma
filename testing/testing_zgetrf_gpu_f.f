!
!   -- MAGMA (version 1.0) --
!      Univ. of Tennessee, Knoxville
!      Univ. of California, Berkeley
!      Univ. of Colorado, Denver
!      November 2010
!
!  @precisions normal z -> c d s
!

      program testing_zgetrf_gpu_f

      external cublas_init, cublas_set_matrix, cublas_get_matrix
      external cublas_shutdown, cublas_alloc
      external zaxpy, zlange, zgetrf, magma_zgetrf_gpu

      double precision zlange
      integer cublas_alloc

      double precision rnumber(2), matnorm, lapnorm, magnorm, work
      complex*16, allocatable         :: h_A(:), h_R(:)
      integer                         :: devptrA
      integer,    allocatable         :: ipiv(:)

      integer i, n, nb, info, lda, size_of_elt

      call cublas_init()

      n   = 2048
      nb  = 128
      lda = n
      size_of_elt = kind(complex*16)
      print *,size_of_elt

!------ Allocate CPU memory
      allocate(h_A(n*n), h_R(n*n), ipiv(n))

!------ Allocate GPU memory
      stat = cublas_alloc(n*n, size_of_elt, devPtrA)
      if (stat .ne. 0) then
         write(*,*) "device memory allocation failed"
         stop
      endif

!---- Initializa the matrix
      do i=1,n*n
        call random_number(rnumber)
        h_A(i) = rnumber(1)
        h_R(i) = rnumber(1)
      end do

!---- devPtrA = h_A
      call cublas_set_matrix (n, n, size_of_elt, h_A, lda, devptrA, lda)

!---- Call magma -------------------
      call magma_zgetrf_gpu(n, n, devptrA, lda, ipiv, info)
      write(*,*) 'exit magma LU'

!---- Call LAPACK ------------------
      call zgetrf(N, N, h_A, lda, ipiv, info)
      write(*,*) 'exit lapack LU'

!---- h_R = devptrA
      call cublas_get_matrix (n, n, size_of_elt, devptrA, lda, h_R, lda)

!---- Compare the two results ------
      lapnorm = zlange('f', n, n, h_A, lda, work)
      magnorm = zlange('f', n, n, h_R, lda, work)

      call zaxpy(n*n, -1., h_A, 1, h_R, 1)
      matnorm = zlange('M', n, n, h_R, lda, work)

      write(*,105) 'Norm Lapack = ', lapnorm
      write(*,105) 'Norm MAGMA  = ', magnorm
      write(*,105) 'error = ', matnorm

!---- Free CPU memory
      deallocate(h_A, h_R, ipiv)

!---- Free GPU memory
      call cublas_free(devPtrA)
      call cublas_shutdown

 105  format((a10,es10.3))

      end
