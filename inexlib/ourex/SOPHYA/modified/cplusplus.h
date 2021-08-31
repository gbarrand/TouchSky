/* G.Barrand : c++ part of the original machdefs. */

#ifndef SOPHYA_CPLUSPLUS_H
#define SOPHYA_CPLUSPLUS_H

/* //////////////////////////////////////////////////////// */
/* /// C++ : ////////////////////////////////////////////// */
/* //////////////////////////////////////////////////////// */
#ifdef __cplusplus

/* The following is true for most compilers */

#ifndef COMPILER_EXCEPTIONS
#define COMPILER_EXCEPTIONS
#endif

#ifndef  CC_HAS_RTTI_SUPPORT 
#define  CC_HAS_RTTI_SUPPORT 
#endif


#ifndef DECL_TEMP_SPEC
#define DECL_TEMP_SPEC
#endif

/* Most compilers dot not implement two level name lookup for templates 
   introduced by gcc >= 3.4 
*/
#ifdef TSNMLUPG4_V1
#undef TSNMLUPG4_V1
#endif
#ifdef TSNMLUPG4
#undef TSNMLUPG4
#endif


/*  Dynamic_cast problem encountered on Mac OS X 10.6 for explicitly instanciated classes across
    dynamic libraries. solved by extern declaration of  explicitly instanciated template classes */
#ifndef NEED_EXT_DECL_TEMP
#define NEED_EXT_DECL_TEMP
#endif


/* now the specific things */

/*  GNU C/C++ compiler specific flags */
#ifdef __GNUC__
#define GCC_VERSION (__GNUC__ * 1000 \
                               + __GNUC_MINOR__ * 100 \
                               + __GNUC_PATCHLEVEL__)
#define ANSI_TEMPLATES
/*
if (__GNUC__ >= 3) 
define STREAMPOS_IS_CLASS
endif
*/
#define GNU_TEMPLATES
#if ( GCC_VERSION >= 3000 ) 
#undef  DECL_TEMP_SPEC
#define DECL_TEMP_SPEC template <>
#endif

/* Two level name look-up scheme for templates introduced from gcc 3.4 */
#if ( GCC_VERSION >= 3400 )
#if ( GCC_VERSION < 4200 )
#define TSNMLUPG4_V1
#else 
#define TSNMLUPG4
#endif
#endif
/* FIN Two level name look-up scheme */
#endif
/*  GNU C/C++ compiler specific flags */

/*  Compilo clang  LLVM  */
#ifdef __clang__ 
#ifndef ANSI_TEMPLATES
#define ANSI_TEMPLATES
#endif 
#undef DECL_TEMP_SPEC
#define DECL_TEMP_SPEC template <>
#undef TSNMLUPG4_V1
#ifndef TSNMLUPG4
#define TSNMLUPG4
#endif
#endif
/*  FIN Compilo clang  LLVM  */

#ifdef __DECCXX
#define ITER_TAG
#define __CXX_PRAGMA_TEMPLATES__
#define PRAGMA_TEMPLATES
#undef NEED_EXT_DECL_TEMP   /* old compiler version cxx 6.5 - we don't activate extern declaration */
#endif

#if defined (__aCC__)  
#ifndef ANSI_TEMPLATES
#define ANSI_TEMPLATES
#endif
#endif

#if defined(__KCC__)
/* KCC V 3.2 ne compile pas  i/ostream s ; long p = s.tellg();  OK en  V 3.3 */
#ifndef ANSI_TEMPLATES
#define ANSI_TEMPLATES
#endif
#endif

#if defined(__SGICC__)
#ifndef ANSI_TEMPLATES
#define ANSI_TEMPLATES
#endif
#undef  DECL_TEMP_SPEC
#define DECL_TEMP_SPEC template <>
#endif

#if defined(__IBMCPP__)
/* IBM xlC compiler support Added : dec 2005 */
#ifndef ANSI_TEMPLATES
#define ANSI_TEMPLATES
#endif
#undef  DECL_TEMP_SPEC
#define DECL_TEMP_SPEC template <>
#endif

/* Get things has homogeneous as possible between compilers 
  Section empty , Reza, Mars 2017  */

#endif    
/* //////////////////////////////////////////////////////// */
/* //////////////////////////////////////////////////////// */
/* //////////////////////////////////////////////////////// */

#endif /*SOPHYA_CPLUSPLUS_H*/
