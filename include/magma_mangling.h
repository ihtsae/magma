/*
    -- MAGMA (version 1.1) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       November 2011
*/

#ifndef MAGMA_MANGLING_H
#define MAGMA_MANGLING_H

#include "mangling.h"

/* Define how to name mangle Fortran names.
 * If using CMake, it defines MAGMA_GLOBAL in mangling.h
 * Otherwise, the make.inc file should have one of -DADD_, -DNOCHANGE, or -DUPCASE.
 * If using outside of MAGMA, put one of those in your compiler flags (e.g., CFLAGS).
 * These macros are used in:
 *   include/magma_*lapack.h
 *   control/magma_*f77.cpp
 */
#ifndef MAGMA_FORTRAN_NAME
    #if defined(MAGMA_GLOBAL)
        #define FORTRAN_NAME(lcname, UCNAME)  MAGMA_GLOBAL( lcname, UCNAME )
    #elif defined(ADD_)
        #define FORTRAN_NAME(lcname, UCNAME)  lcname##_
    #elif defined(NOCHANGE)
        #define FORTRAN_NAME(lcname, UCNAME)  lcname
    #elif defined(UPCASE)
        #define FORTRAN_NAME(lcname, UCNAME)  UCNAME
    #else
        #error Define one of ADD_, NOCHANGE, or UPCASE (in make.inc, or in your CFLAGS) for how Fortran functions are name mangled. If using CMake, it should define MAGMA_GLOBAL.
    #endif
#endif

#endif        //  #ifndef MAGMA_MANGLING_H
