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
    ('zlange','dlange'),
    ('zlag2c','dlag2s'),
    ('clag2z','slag2d'),
    ('zlat2c','dlat2s'),
    ('clat2z','slat2d'),
    ('zlacpy','dlacpy'),
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
    ('zlanhe','dlansy'),
    ('zlaghe','dlagsy'),
    ('ztrmm','dtrmm'),
    ('ctrmm','strmm'),
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
  ],
  'normal' : [ ## Dictionary is keyed on substitution type
    ['s','d','c','z'], ## Special Line Indicating type columns
    ('#define REAL','#define REAL','#define COMPLEX','#define COMPLEX'),
    ('#undef COMPLEX','#undef COMPLEX','#undef REAL','#undef REAL'),
    ('#define SINGLE','#define DOUBLE','#define SINGLE','#define DOUBLE'),
    ('#undef DOUBLE', '#undef SINGLE', '#undef DOUBLE', '#undef SINGLE' ),
    ('Workspace_s','Workspace_d','Workspace_c','Workspace_z'),
    ('workspace_s','workspace_d','workspace_c','workspace_z'),
    ('cublasS','cublasD','cublasC','cublasZ'),
    ('spanel_to_q','dpanel_to_q','cpanel_to_q','zpanel_to_q'),
    ('sq_to_panel','dq_to_panel','cq_to_panel','zq_to_panel'),
    ('cblas_sasum','cblas_dasum','cblas_scasum','cblas_dzasum'),
    ('CORE_sasum','CORE_dasum','CORE_scasum','CORE_dzasum'),
    ('core_sasum','core_dasum','core_scasum','core_dzasum'),
    ('slag2d','dlag2s','clag2z','zlag2c'),
    ('scopy','dcopy','ccopy','zcopy'),
    ('strmv','dtrmv','ctrmv','ztrmv'),
    ('sgemv','dgemv','cgemv','zgemv'),
    ('ssymv','dsymv','csymv','zsymv'),
    ('sscal','dscal','cscal','zscal'),
    ('sdot','ddot','cdot','zdot'),
    ('sbdt01','dbdt01','cbdt01','zbdt01'),
    ('saxpy','daxpy','caxpy','zaxpy'),
    ('ssymm','dsymm','chemm','zhemm'),
    ('SSYMM','DSYMM','CHEMM','ZHEMM'),
    ('ssyrk','dsyrk','cherk','zherk'),
    ('SSYRK','DSYRK','CHERK','ZHERK'),
    ('sgesv','dgesv','cgesv','zgesv'),
    ('sgeqlf','dgeqlf','cgeqlf','zgeqlf'),
    ('sgelqf','dgelqf','cgelqf','zgelqf'),
    ('sgebrd','dgebrd','cgebrd','zgebrd'),
    ('sgehrd','dgehrd','cgehrd','zgehrd'),
    ('sgehd2','dgehd2','cgehd2','zgehd2'),
    ('SUNGESV','SUNGESV','CUNGESV','CUNGESV'),
    ('SGESV','SGESV','CGESV','CGESV'),
    ('SGESV','DGESV','CGESV','ZGESV'),
    ('sgels','dgels','cgels','zgels'),
    ('SGELS','DGELS','CGELS','ZGELS'),
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
    ('slacgv','dlacgv','clacgv','zlacgv'),
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
    ('slarft','dlarft','clarft','zlarft'),
    ('slarnv','dlarnv','clarnv','zlarnv'),
    ('slaswp','dlaswp','claswp','zlaswp'),
    ('splgsy','dplgsy','cplghe','zplghe'),
    ('spotrf','dpotrf','cpotrf','zpotrf'),
    ('spotrf','dpotrf','cpotrf','zpotrf'),
    ('SPOTRF','DPOTRF','CPOTRF','ZPOTRF'),
    ('spotrs','dpotrs','cpotrs','zpotrs'),
    ('SPOTRS','DPOTRS','CPOTRS','ZPOTRS'),
    ('sorglq','dorglq','cunglq','zunglq'),
    ('SORGLQ','DORGLQ','CUNGLQ','ZUNGLQ'),
    ('sorg2r','dorg2r','cung2r','zung2r'),
    ('sorgqr','dorgqr','cungqr','zungqr'),
    ('SORGQR','DORGQR','CUNGQR','ZUNGQR'),
    ('sormlq','dormlq','cunmlq','zunmlq'),
    ('SORMLQ','DORMLQ','CUNMLQ','ZUNMLQ'),
    ('sormqr','dormqr','cunmqr','zunmqr'),
    ('SORMQR','DORMQR','CUNMQR','ZUNMQR'),
    ('slamch','dlamch','slamch','dlamch'),
    ('slarnv','dlarnv','slarnv','dlarnv'),
    ('slauum','dlauum','clauum','zlauum'),
    ('spotri','dpotri','cpotri','zpotri'),
    ('strtri','dtrtri','ctrtri','ztrtri'),
    ('slatrd','dlatrd','clatrd','zlatrd'),
    ('slahr', 'dlahr', 'clahr', 'zlahr'),
    ('slabrd','dlabrd','clabrd','zlabrd'),
    ('stsqrt','dtsqrt','ctsqrt','ztsqrt'),
    ('ssytrd','dsytrd','csytrd','zsytrd'),
    ('sormtr','dormtr','cunmtr','zunmtr'),
    ('strsmpl','dtrsmpl','ctrsmpl','ztrsmpl'),
    ('STRSMPL','DTRSMPL','CTRSMPL','ZTRSMPL'),
    ('\*\*T','\*\*T','\*\*H','\*\*H'),
    ('BLAS_s','BLAS_d','BLAS_s','BLAS_d'),
    ('BLAS_s','BLAS_d','BLAS_c','BLAS_z'),
    ('fabsf','fabs','cabsf','cabs'),
    ('cblas_is','cblas_id','cblas_ic','cblas_iz'),
    ('cblas_s','cblas_d','cblas_c','cblas_z'),
    ('','','CBLAS_SADDR','CBLAS_SADDR'),
    ('CblasTrans','CblasTrans','CblasConjTrans','CblasConjTrans'),
    ('REAL','DOUBLE_PRECISION','COMPLEX','COMPLEX_16'),
    ('','','conjf','conj'),
    ('CORE_S','CORE_D','CORE_C','CORE_Z'),
    ('CORE_s','CORE_d','CORE_c','CORE_z'),
    ('CORE_s','CORE_d','CORE_s','CORE_d'),
    ('core_s','core_d','core_c','core_z'),
    ('example_s','example_d','example_c','example_z'),
    ('float','double','float2','double2'),
    ('float','double','float','double'),
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
    ('ger','ger','gerc','gerc'),
    ('ger','ger','geru','geru'),
    ('symm','symm','hemm','hemm'),
    ('syrk','syrk','herk','herk'),
    ('lansy','lansy','lanhe','lanhe'),
    ('stile','dtile','ctile','ztile'),
    ('szero','dzero','czero','zzero'),
  ],
  'tracing' : [
    ['plain','tau'],
    ('(\w+\*?)\s+(\w+)\s*\(([a-z* ,A-Z_0-9]*)\)\s*{\s+(.*)\s*#pragma tracing_start\s+(.*)\s+#pragma tracing_end\s+(.*)\s+}',r'\1 \2(\3){\n\4tau("\2");\5tau();\6}'),
    ('\.c','.c.tau'),
  ],
};
