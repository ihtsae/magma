subs = {
  'all' : [ ## Special key: Changes are applied to all applicable conversions automatically
    [None,None]
  ],
  'mixed' : [
    ['zc','ds'],
    ('ZC','DS'),
    ('zc','ds'),
    ('PLASMA_Complex64_t','double'),
    ('PLASMA_Complex32_t','float'),
    ('PlasmaComplexDouble','PlasmaRealDouble'),
    ('PlasmaComplexFloat','PlasmaRealFloat'),
    ('zlange','dlange'),
    ('zlag2c','dlag2s'),
    ('clag2z','slag2d'),
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
    ('PLASMA_Alloc_Workspace_zgels','PLASMA_Alloc_Workspace_dgels'),
    ('pcgeqrf','psgeqrf'),
    ('pcunmqr','psormqr'),
    ('pcgelqf','psgelqf'),
    ('pcunmlq','psormlq'),
    ('pzgeqrf','pdgeqrf'),
    ('pzunmqr','pdormqr'),
    ('pzgelqf','pdgelqf'),
    ('pzunmlq','pdormlq'),
    ('plasma_pclapack','plasma_pslapack'),
    ('plasma_pzlapack','plasma_pdlapack'),
    ('plasma_pctile',  'plasma_pstile'),
    ('plasma_pztile',  'plasma_pdtile'),
  ],
  'normal' : [ ## Dictionary is keyed on substitution type
    ['s','d','c','z'], ## Special Line Indicating type columns
    ('#define REAL','#define REAL','#define COMPLEX','#define COMPLEX'),
    ('#undef COMPLEX','#undef COMPLEX','#undef REAL','#undef REAL'),
    ('#define SINGLE','#define DOUBLE','#define SINGLE','#define DOUBLE'),
    ('#undef DOUBLE', '#undef SINGLE', '#undef DOUBLE', '#undef SINGLE' ),
    ('Workspace_s','Workspace_d','Workspace_c','Workspace_z'),
    ('workspace_s','workspace_d','workspace_c','workspace_z'),
    ('cblas_sasum','cblas_dasum','cblas_scasum','cblas_dzasum'),
    ('CORE_sasum','CORE_dasum','CORE_scasum','CORE_dzasum'),
    ('core_sasum','core_dasum','core_scasum','core_dzasum'),
    ('slag2d','dlag2s','clag2z','zlag2c'),
    ('saxpy','daxpy','caxpy','zaxpy'),
    ('ssymm','dsymm','chemm','zhemm'),
    ('SSYMM','DSYMM','CHEMM','ZHEMM'),
    ('ssyrk','dsyrk','cherk','zherk'),
    ('SSYRK','DSYRK','CHERK','ZHERK'),
    ('sgesv','dgesv','cgesv','zgesv'),
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
    ('float','double','float _Complex','double _Complex'),
    ('float','double','float','double'),
    ('example_s','example_d','example_c','example_z'),
    ('lapack_s','lapack_d','lapack_c','lapack_z'),
    ('lapack_slamch','lapack_dlamch','lapack_slamch','lapack_dlamch'),
    ('float','double','PLASMA_Complex32_t','PLASMA_Complex64_t'),
    ('plasma_ps','plasma_pd','plasma_pc','plasma_pz'),
    ('float','double','PLASMA_voidComplex32_t','PLASMA_voidComplex64_t'),
    ('PLASMA_s','PLASMA_d','PLASMA_c','PLASMA_z'),
    ('PLASMA_S','PLASMA_D','PLASMA_C','PLASMA_Z'),
    ('plasma_s','plasma_d','plasma_c','plasma_z'),
    ('control_s','control_d','control_c','control_z'),
    ('compute_s','compute_d','compute_c','compute_z'),
    ('PLASMA_sor','PLASMA_dor','PLASMA_cun','PLASMA_zun'),
    ('PlasmaRealFloat','PlasmaRealDouble','PlasmaComplexFloat','PlasmaComplexDouble'),
    ('PlasmaTrans','PlasmaTrans','PlasmaConjTrans','PlasmaConjTrans'),
    ('testing_ds','testing_ds','testing_zc','testing_zc'),
    ('testing_s','testing_d','testing_c','testing_z'),
    ('TESTING_S','TESTING_D','TESTING_C','TESTING_Z'),
    ('stesting','dtesting','ctesting','ztesting'),
    ('SAUXILIARY','DAUXILIARY','CAUXILIARY','ZAUXILIARY'),
    ('sauxiliary','dauxiliary','cauxiliary','zauxiliary'),
    ('scheck','dcheck','ccheck','zcheck'),
    ('ger','ger','gerc','gerc'),
    ('ger','ger','geru','geru'),
    ('symm','symm','hemm','hemm'),
    ('syrk','syrk','herk','herk'),
    ('lansy','lansy','lanhe','lanhe'),
    ('stile','dtile','ctile','ztile'),
  ],
  'tracing' : [
    ['plain','tau'],
    ('(\w+\*?)\s+(\w+)\s*\(([a-z* ,A-Z_0-9]*)\)\s*{\s+(.*)\s*#pragma tracing_start\s+(.*)\s+#pragma tracing_end\s+(.*)\s+}',r'\1 \2(\3){\n\4tau("\2");\5tau();\6}'),
    ('\.c','.c.tau'),
  ],
};
