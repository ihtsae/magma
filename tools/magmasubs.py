subs = {
  'all' : [ ## Special key: Changes are applied to all applicable conversions automatically
    [None,None]
  ],
  'mixed' : [
    ['zc','ds'],
    ('ZC','DS'),
    ('zc','ds'),
    ('double2','double'),
    ('float2','float'),
    ('cuDoubleComplex','double'),
    ('cuFloatComplex','float'),
    ('cuCrealf',''),
    ('cuCimagf',''),
    ('cuCreal',''),
    ('cuCimag',''),
    ('zlange','dlange'),
    ('zlag2c','dlag2s'),
    ('clag2z','slag2d'),
    ('zlat2c','dlat2s'),
    ('clat2z','slat2d'),
    ('zlacpy','dlacpy'),
    ('zclaswp','dslaswp'),
    ('zgemm','dgemm'),
    ('zherk','dsyrk'),
    ('zlansy','dlansy'),
    ('zaxpy','daxpy'),
    ('pzgetrf','pdgetrf'),
    ('pcgetrf','psgetrf'),
    ('ztrsm','dtrsm'),
    ('ctrsm','strsm'),
    ('CBLAS_SADDR',''),
    ('zlarnv','dlarnv'),
    ('zgesv','dgesv'),
    ('zhemm','dsymm'),
    ('Zhemm','Dsymm'),
    ('zhemv','dsymv'),
    ('Zhemv','Dsymv'),
    ('zlanhe','dlansy'),
    ('clanhe','slansy'),
    ('zlaghe','dlagsy'),
    ('ztrmm','dtrmm'),
    ('ctrmm','strmm'),
    ('zunmqr','dormqr'),
    ('cuConj',''),
    ('Conj',''),
    ('zpotrf','dpotrf'),
    ('cpotrf','spotrf'),
    ('pcgeqrf','psgeqrf'),
    ('pcunmqr','psormqr'),
    ('pcgelqf','psgelqf'),
    ('pcunmlq','psormlq'),
    ('pzgeqrf','pdgeqrf'),
    ('pzunmqr','pdormqr'),
    ('pzgelqf','pdgelqf'),
    ('pzunmlq','pdormlq'),
    ('MAGMA_ZC','MAGMA_DS'),
    ('MAGMABLAS_ZC','MAGMABLAS_DS'),
    ('magma_z','magma_d'),
    ('magma_czgetrs','magma_sdgetrs'),
    ('magma_c','magma_s'),
    ('magmablas_z','magmablas_d'),
    ('magmablas_c','magmablas_s'),
    ('cublasZ','cublasD'),
    ('cublasC','cublasS'),
    ('MAGMA_Z','MAGMA_D'),
    ('MAGMA_C','MAGMA_S'),
    ('cublasIzamax','cublasIdamax'),
    ('lapackf77_z','lapackf77_d'),
    ('lapackf77_c','lapackf77_s'),
    ('blasf77_z','blasf77_d'),
    ('blasf77_c','blasf77_s'),
    ('cuComplexFloatToDouble','(double)'),
    ('cuComplexDoubleToFloat','(float)'),
    ('cuCabs', 'abs' ),  
    ('cuCabsf','absf'),
    ('zcaxpy','dsaxpy'),
    ('magma_get_c','magma_get_s'),
    ('magma_get_z','magma_get_d'),
    ('#define PRECISION_z', '#define PRECISION_d'),
    ('#define PRECISION_c', '#define PRECISION_s'),
    ('#undef PRECISION_z', '#undef PRECISION_d'),
    ('#undef PRECISION_c', '#undef PRECISION_s'),
    ],
  'normal' : [ ## Dictionary is keyed on substitution type
    ['s','d','c','z'], ## Special Line Indicating type columns
    ('#define PRECISION_s', '#define PRECISION_d', '#define PRECISION_c', '#define PRECISION_z' ),
    ('#undef PRECISION_s', '#undef PRECISION_d', '#undef PRECISION_c', '#undef PRECISION_z' ),
    ('#define REAL','#define REAL','#define COMPLEX','#define COMPLEX'),
    ('#undef COMPLEX','#undef COMPLEX','#undef REAL','#undef REAL'),
    ('#define SINGLE','#define DOUBLE','#define SINGLE','#define DOUBLE'),
    ('#undef DOUBLE', '#undef SINGLE', '#undef DOUBLE', '#undef SINGLE' ),
    ('real','double precision','complex','complex\*16'),
    ('real','double precision','real','double precision'),
    ('geev_wr_array, float *geev_wi_array','geev_wr_array, double *geev_wi_array','geev_w_array','geev_w_array'),
    ('sizeof_real','sizeof_double','sizeof_complex','sizeof_complex_16'),
    ('float','double','cuFloatComplex','cuDoubleComplex'),
    ('Workspace_s','Workspace_d','Workspace_c','Workspace_z'),
    ('workspace_s','workspace_d','workspace_c','workspace_z'),
    ('lapackf77_s','lapackf77_d','lapackf77_c','lapackf77_z'),
    ('lapackf77_s','lapackf77_d','lapackf77_s','lapackf77_d'),
    ('sdot_sub','ddot_sub','cdotu_sub','zdotu_sub'),
    ('sdot_sub','ddot_sub','cdotc_sub','zdotc_sub'),
    ('sdot','ddot','cdotc','zdotc'),
    ('blasf77_s','blasf77_d','blasf77_c','blasf77_z'),
    ('blasf77_s','blasf77_d','blasf77_s','blasf77_d'),
    ('cublasS','cublasD','cublasC','cublasZ'),
    ('cublasIsamax','cublasIdamax', 'cublasIcamax','cublasIzamax'),
    ('cublasIsamax','cublasIdamax', 'cublasIsamax','cublasIdamax'),
    ('cpu_gpu_s','cpu_gpu_d','cpu_gpu_c','cpu_gpu_z'),
    ('spanel_to_q','dpanel_to_q','cpanel_to_q','zpanel_to_q'),
    ('sq_to_panel','dq_to_panel','cq_to_panel','zq_to_panel'),
    ('cblas_sasum','cblas_dasum','cblas_scasum','cblas_dzasum'),
    ('CORE_sasum','CORE_dasum','CORE_scasum','CORE_dzasum'),
    ('core_sasum','core_dasum','core_scasum','core_dzasum'),
    ('','','cuCrealf','cuCreal'),
    ('','','cuCimagf','cuCimag'),
    ('slag2d','dlag2s','clag2z','zlag2c'),
    ('sinplace','dinplace','cinplace','zinplace'),
    ('spermute','dpermute','cpermute','zpermute'),
    ('stranspose','dtranspose','ctranspose','ztranspose'),
    ('ssplit','dsplit','csplit','zsplit'),
    ('SSIZE','DSIZE','CSIZE','ZSIZE'),
    ('scopy','dcopy','ccopy','zcopy'),
    ('strmv','dtrmv','ctrmv','ztrmv'),
    ('sgemv','dgemv','cgemv','zgemv'),
    ('ssymv','dsymv','csymv','zsymv'),
    ('sscal','dscal','cscal','zscal'),
    ('ssyr2k','dsyr2k','cher2k','zher2k'),
    ('ssyr2k','dsyr2k','csyr2k','zsyr2k'),
    ('sbdt01','dbdt01','cbdt01','zbdt01'),
    ('saxpy','daxpy','caxpy','zaxpy'),
    ('ssymv','dsymv','chemv','zhemv'),
    ('SSYMV','DSYMV','CHEMV','ZHEMV'),
    ('ssymm','dsymm','chemm','zhemm'),
    ('SSYMM','DSYMM','CHEMM','ZHEMM'),
    ('ssyrk','dsyrk','cherk','zherk'),
    ('SSYRK','DSYRK','CHERK','ZHERK'),
    ('sgesv','dgesv','cgesv','zgesv'),
    ('sgeqlf','dgeqlf','cgeqlf','zgeqlf'),
    ('sgelqf','dgelqf','cgelqf','zgelqf'),
    ('sgebrd','dgebrd','cgebrd','zgebrd'),
    ('sgehrd','dgehrd','cgehrd','zgehrd'),
    ('SGEHRD','DGEHRD','CGEHRD','ZGEHRD'),
    ('sgehd2','dgehd2','cgehd2','zgehd2'),
    ('sgebd2','dgebd2','cgebd2','zgebd2'),
    ('SUNGESV','SUNGESV','CUNGESV','CUNGESV'),
    ('SGESV','SGESV','CGESV','CGESV'),
    ('SGESV','DGESV','CGESV','ZGESV'),
    ('sgels','dgels','cgels','zgels'),
    ('SGELS','DGELS','CGELS','ZGELS'),
    ('sgeev','dgeev','cgeev','zgeev'),
    ('SGEEV','DGEEV','CGEEV','ZGEEV'),
    ('ssyev','dsyevd','cheevd','zheevd'),
    ('SSYEV','DSYEVD','CHEEVD','ZHEEVD'),
    ('sgemm','dgemm','cgemm','zgemm'),
    ('SGEMM','DGEMM','CGEMM','ZGEMM'),
    ('sposv','dposv','cposv','zposv'),
    ('SPOSV','SPOSV','CPOSV','CPOSV'),
    ('SPOSV','DPOSV','CPOSV','ZPOSV'),
    ('ssymm','dsymm','csymm','zsymm'),
    ('SSYMM','DSYMM','CSYMM','ZSYMM'),
    ('ssyrk','dsyrk','csyrk','zsyrk'),
    ('SSYRK','DSYRK','CSYRK','ZSYRK'),
    ('strmm','dtrmm','ctrmm','ztrmm'),
    ('STRMM','DTRMM','CTRMM','ZTRMM'),
    ('strsm','dtrsm','ctrsm','ztrsm'),
    ('STRSM','DTRSM','CTRSM','ZTRSM'),
    ('sgelq2','dgelq2','cgelq2','zgelq2'),
    ('sgelqf','dgelqf','cgelqf','zgelqf'),
    ('SGELQF','DGELQF','CGELQF','ZGELQF'),
    ('SGEBRD','DGEBRD','CGEBRD','ZGEBRD'),
    ('sgelqs','dgelqs','cgelqs','zgelqs'),
    ('SGELQS','DGELQS','CGELQS','ZGELQS'),
    ('sgeqr2','dgeqr2','cgeqr2','zgeqr2'),
    ('sgeqrf','dgeqrf','cgeqrf','zgeqrf'),
    ('SGEQRF','DGEQRF','CGEQRF','ZGEQRF'),
    ('sgeqrs','dgeqrs','cgeqrs','zgeqrs'),
    ('SGEQRS','DGEQRS','CGEQRS','ZGEQRS'),
    ('sgetf2','dgetf2','cgetf2','zgetf2'),
    ('sgetrf','dgetrf','cgetrf','zgetrf'),
    ('SGETRF','DGETRF','CGETRF','ZGETRF'),
    ('sgetrs','dgetrs','cgetrs','zgetrs'),
    ('SGETRS','DGETRS','CGETRS','ZGETRS'),
    ('slaset','dlaset','claset','zlaset'),
    ('slascl','dlascl','clascl','zlascl'),
    ('sgebak','dgebak','cgebak','zgebak'),
    ('sgebal','dgebal','cgebal','zgebal'),
    ('sscal','dscal','csscal','zdscal'),
    ('shseqr','dhseqr','chseqr','zhseqr'),
    ('strevc','dtrevc','ctrevc','ztrevc'),
    ('slacgv','dlacgv','clacgv','zlacgv'),
    ('slabad','dlabad','slabad','dlabad'),
    ('isamax','idamax','isamax','idamax'),
    ('snrm2','dnrm2','scnrm2','dznrm2'),
    ('slacpy','dlacpy','clacpy','zlacpy'),
    ('slagsy','dlagsy','claghe','zlaghe'),
    ('slagsy','dlagsy','clagsy','zlagsy'),
    ('SLANGE','DLANGE','CLANGE','ZLANGE'),
    ('SLANSY','DLANSY','CLANHE','ZLANHE'),
    ('SLANSY','DLANSY','CLANSY','ZLANSY'),
    ('SLANTR','DLANTR','CLANTR','ZLANTR'),
    ('slange','dlange','clange','zlange'),
    ('slansy','dlansy','clanhe','zlanhe'),
    ('slansy','dlansy','clansy','zlansy'),
    ('slantr','dlantr','clantr','zlantr'),
    ('slarfb','dlarfb','clarfb','zlarfb'),
    ('slarfg','dlarfg','clarfg','zlarfg'),
    ('slarfy','dlarfy','clarfy','zlarfy'),
    ('slarft','dlarft','clarft','zlarft'),
    ('slarnv','dlarnv','clarnv','zlarnv'),
    ('slaswp','dlaswp','claswp','zlaswp'),
    ('sswap','dswap','cswap','zswap'),
    ('splgsy','dplgsy','cplghe','zplghe'),
    ('spotrf','dpotrf','cpotrf','zpotrf'),
    ('spotrf','dpotrf','cpotrf','zpotrf'),
    ('SPOTRF','DPOTRF','CPOTRF','ZPOTRF'),
    ('spotrs','dpotrs','cpotrs','zpotrs'),
    ('SPOTRS','DPOTRS','CPOTRS','ZPOTRS'),
    ('sort01','dort01','cunt01','zunt01'),
    ('sorm2r','dorm2r','cunm2r','zunm2r'),
    ('sormr2','dormr2','cunmr2','zunmr2'),
    ('sorglq','dorglq','cunglq','zunglq'),
    ('sorghr','dorghr','cunghr','zunghr'),
    ('SORGHR','DORGHR','CUNGHR','ZUNGHR'),
    ('orghr', 'orghr', 'unghr', 'unghr'),
    ('SORGLQ','DORGLQ','CUNGLQ','ZUNGLQ'),
    ('sorg2r','dorg2r','cung2r','zung2r'),
    ('sorgqr','dorgqr','cungqr','zungqr'),
    ('SORGQR','DORGQR','CUNGQR','ZUNGQR'),
    ('sorgtr','dorgtr','cungtr','zungtr'),
    ('sorgbr','dorgbr','cungbr','zungbr'),
    ('sormlq','dormlq','cunmlq','zunmlq'),
    ('sormql','dormql','cunmql','zunmql'),
    ('SORMLQ','DORMLQ','CUNMLQ','ZUNMLQ'),
    ('sormqr','dormqr','cunmqr','zunmqr'),
    ('SORMQR','DORMQR','CUNMQR','ZUNMQR'),
    ('sormr2','dormr2','cunmr2','zunmr2'),
    ('slamch','dlamch','slamch','dlamch'),
    ('slarnv','dlarnv','slarnv','dlarnv'),
    ('slauum','dlauum','clauum','zlauum'),
    ('spotri','dpotri','cpotri','zpotri'),
    ('strtri','dtrtri','ctrtri','ztrtri'),
    ('slatrd','dlatrd','clatrd','zlatrd'),
    ('slahr', 'dlahr', 'clahr', 'zlahr'),
    ('slabrd','dlabrd','clabrd','zlabrd'),
    ('SLABRD','DLABRD','CLABRD','ZLABRD'),
    ('stsqrt','dtsqrt','ctsqrt','ztsqrt'),
    ('sqrt02','dqrt02','cqrt02','zqrt02'),
    ('ssytrd','dsytrd','chetrd','zhetrd'),
    ('SSYTRD','DSYTRD','CHETRD','ZHETRD'),
    ('ssytd2','dsytd2','chetd2','zhetd2'),
    ('ssyt21','dsyt21','chet21','zhet21'),
    ('shst01','dhst01','chst01','zhst01'),
    ('sstt21','dstt21','cstt21','zstt21'),
    ('sormtr','dormtr','cunmtr','zunmtr'),
    ('SORMTR','DORMTR','CUNMTR','ZUNMTR'),
    ('strsmpl','dtrsmpl','ctrsmpl','ztrsmpl'),
    ('STRSMPL','DTRSMPL','CTRSMPL','ZTRSMPL'),
    ('\*\*T','\*\*T','\*\*H','\*\*H'),
    ('BLAS_s','BLAS_d','BLAS_s','BLAS_d'),
    ('BLAS_s','BLAS_d','BLAS_c','BLAS_z'),
    ('BLAS_S','BLAS_D','BLAS_C','BLAS_Z'),
    ('blas_s','blas_d','blas_c','blas_z'),
    ('fabsf','fabs','cabsf','cabs'),
    ('fabsf','fabs','cuCabsf','cuCabs'),
    ('cblas_is','cblas_id','cblas_ic','cblas_iz'),
    ('cblas_s','cblas_d','cblas_c','cblas_z'),
    ('','','CBLAS_SADDR','CBLAS_SADDR'),
    ('CblasTrans','CblasTrans','CblasConjTrans','CblasConjTrans'),
    ('MagmaTrans','MagmaTrans','MagmaConjTrans','MagmaConjTrans'),
    ('REAL','DOUBLE_PRECISION','COMPLEX','COMPLEX_16'),
    ('symmetric','symmetric','Hermitian','Hermitian'),
    ('','','conjf','conj'),
    ('','','cuConjf','cuConj'),
    ('CORE_S','CORE_D','CORE_C','CORE_Z'),
    ('CORE_s','CORE_d','CORE_c','CORE_z'),
    ('CORE_s','CORE_d','CORE_s','CORE_d'),
    ('core_s','core_d','core_c','core_z'),
    ('example_s','example_d','example_c','example_z'),
    ('float','double','float2','double2'),
    ('float','double','float','double'),
    ('float','double','cuFloatComplex','cuDoubleComplex'),
    ('control_s','control_d','control_c','control_z'),
    ('compute_s','compute_d','compute_c','compute_z'),
    ('testing_ds','testing_ds','testing_zc','testing_zc'),
    ('testing_s','testing_d','testing_c','testing_z'),
    ('TESTING_S','TESTING_D','TESTING_C','TESTING_Z'),
    ('stesting','dtesting','ctesting','ztesting'),
    ('SAUXILIARY','DAUXILIARY','CAUXILIARY','ZAUXILIARY'),
    ('sauxiliary','dauxiliary','cauxiliary','zauxiliary'),
    ('scheck','dcheck','ccheck','zcheck'),
    ('MAGMA_S','MAGMA_D','MAGMA_C','MAGMA_Z'),
    ('MAGMABLAS_S','MAGMABLAS_D','MAGMABLAS_C','MAGMABLAS_Z'),
    ('magma_s','magma_d','magma_c','magma_z'),
    ('magmablas_s','magmablas_d','magmablas_c','magmablas_z'),
    ('magma_get_s','magma_get_d','magma_get_c','magma_get_z'),
    ('ger','ger','gerc','gerc'),
    ('ger','ger','geru','geru'),
    ('orgtr','orgtr','ungtr','ungtr'),
    ('orgbr','orgbr','ungbr','ungbr'),
    ('ort01','ort01','unt01','unt01'),
    ('ormqr','ormqr','unmqr','unmqr'),
    ('ormlq','ormlq','unmlq','unmlq'),
    ('ormql','ormql','unmql','unmql'),
    ('ormr2','ormr2','unmr2','unmr2'),
    ('orm2r','orm2r','unm2r','unm2r'),
    ('org2r','org2r','ung2r','ung2r'),
    ('orgqr','orgqr','ungqr','ungqr'),
    ('symm','symm','hemm','hemm'),
    ('gemm','gemm','gemm','gemm'),
    ('symv','symv','hemv','hemv'),
    ('syrk','syrk','herk','herk'),
    ('syr2k','syr2k','her2k','her2k'),
    ('lansy','lansy','lanhe','lanhe'),
    ('sytd2','sytd2','hetd2','hetd2'),
    ('sytrd','sytrd','hetrd','hetrd'),
    ('syt21','syt21','het21','het21'),
    ('stile','dtile','ctile','ztile'),
    ('szero','dzero','czero','zzero'),
    ('sset','dset','cset','zset'),
	('SCHED_s','SCHED_d','SCHED_c','SCHED_z'),
	('QUARK_Insert_Task_s','QUARK_Insert_Task_d','QUARK_Insert_Task_c','QUARK_Insert_Task_z'),
  ],
  'tracing' : [
    ['plain','tau'],
    ('(\w+\*?)\s+(\w+)\s*\(([a-z* ,A-Z_0-9]*)\)\s*{\s+(.*)\s*#pragma tracing_start\s+(.*)\s+#pragma tracing_end\s+(.*)\s+}',r'\1 \2(\3){\n\4tau("\2");\5tau();\6}'),
    ('\.c','.c.tau'),
  ],
};
