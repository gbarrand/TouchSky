/* G.Barrand */

#define H5_PACKAGE_BUGREPORT "hdfhelp@ncsa.uiuc.edu"
#define H5_PACKAGE_NAME "HDF5"
#define H5_PACKAGE_STRING "HDF5 1.6.5"
#define H5_PACKAGE_TARNAME "hdf5"
#define H5_PACKAGE_VERSION "1.6.5"

/* //////////////////////////////////////////////////////// */
/* /// sizes ////////////////////////////////////////////// */
/* //////////////////////////////////////////////////////// */

/* (expected to be) universal sizes : */
#define H5_SIZEOF_CHAR   1
#define H5_SIZEOF_SHORT  2
#define H5_SIZEOF_INT    4
#define H5_SIZEOF_FLOAT  4
#define H5_SIZEOF_DOUBLE 8

/* //////////////////////////////////////////////////////// */
/* /// __APPLE__ : //////////////////////////////////////// */
/* //////////////////////////////////////////////////////// */
#if defined(__APPLE__) 

#define H5_HAVE_UNISTD_H 1
#define H5_HAVE_STDINT_H 1

#include <TargetConditionals.h>

#if TARGET_OS_IPHONE

#if defined(TARGET_RT_64_BIT) && (TARGET_RT_64_BIT == 1) 
#define H5_OUREX_64
#else
#define H5_OUREX_32
#endif

#else /*macOS*/

#if defined(TARGET_RT_64_BIT) && (TARGET_RT_64_BIT == 1) 
#define H5_OUREX_64
/* Do we need the four below lines ? If needed, inlib/app/sizeof.cpp can help with that.
#define H5_SIZEOF_INT_FAST16_T  2
#define H5_SIZEOF_INT_FAST32_T  4
#define H5_SIZEOF_UINT_FAST16_T 2
#define H5_SIZEOF_UINT_FAST32_T 4
*/
#define H5_HAVE_INTTYPES_H 1
#else
#define H5_OUREX_32
#endif
#define H5_HAVE_THREADSAFE 1   /*add H5TS.c*/

#if defined(TARGET_RT_BIG_ENDIAN)  && (TARGET_RT_BIG_ENDIAN == 1) 
#error "ourex/hdf5/include/H5pubconf.h : __APPLE__ platform not handled."
#endif

#endif /*TARGET_OS_IPHONE*/

/* //////////////////////////////////////////////////////// */
/* /// ANDROID : ////////////////////////////////////////// */
/* //////////////////////////////////////////////////////// */
#elif defined(ANDROID)

/* Samsung Galaxy Note 8 is 64 bits.*/
/* Samsung Galaxy S1 is 32 bits and little endian. */

#if defined(__arm__)
  #define H5_OUREX_32
/*
  #if defined(__ARM_ARCH_7A__)
    #if defined(__ARM_NEON__)
      #if defined(__ARM_PCS_VFP)
      #else
      #endif
    #else
      #if defined(__ARM_PCS_VFP)
      #else
      #endif
    #endif
  #else
  #endif
*/
#elif defined(__i386__)
#define H5_OUREX_32
#elif defined(__x86_64__)
#define H5_OUREX_64
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
#define H5_OUREX_64
#elif defined(__mips__)
#define H5_OUREX_32
#elif defined(__aarch64__)
#define H5_OUREX_64
#else
#error "ourex/hdf5/include/H5pubconf.h : unknown ANDROID platform."
#endif

#define H5_HAVE_UNISTD_H 1
#define H5_HAVE_STDINT_H 1

/* //////////////////////////////////////////////////////// */
/* /// __linux__ : //////////////////////////////////////// */
/* //////////////////////////////////////////////////////// */
#elif defined(__linux__)

#define H5_HAVE_UNISTD_H 1
#define H5_HAVE_STDINT_H 1

#define H5_HAVE_THREADSAFE 1   /*add H5TS.c*/

#if defined(__x86_64__)
/*#elif defined(_LP64) ??? */
#define H5_OUREX_64

#define H5_SIZEOF_INT_FAST16_T 8
#define H5_SIZEOF_INT_FAST32_T 8
#define H5_SIZEOF_UINT_FAST16_T 8
#define H5_SIZEOF_UINT_FAST32_T 8

#else
#define H5_OUREX_32
#endif

/* //////////////////////////////////////////////////////// */
/* /// _MSC_VER : ///////////////////////////////////////// */
/* //////////////////////////////////////////////////////// */
#elif defined(_MSC_VER)

#define H5_SIZEOF_LONG        4
#define H5_SIZEOF_LONG_LONG   8
#define H5_SIZEOF_LONG_DOUBLE 8
#define H5_SIZEOF_OFF_T       4

#define H5_SIZEOF_SHORT       2

#define H5_SIZEOF_INT8_T      1
#define H5_SIZEOF_INT16_T     2
#define H5_SIZEOF_INT32_T     4
#define H5_SIZEOF_INT64_T     8

#define H5_SIZEOF_UINT8_T     1
#define H5_SIZEOF_UINT16_T    2
#define H5_SIZEOF_UINT32_T    4
#define H5_SIZEOF_UINT64_T    8

#define H5_SIZEOF_INT_FAST8_T  1
#define H5_SIZEOF_INT_FAST16_T 4
#define H5_SIZEOF_INT_FAST32_T 4
#define H5_SIZEOF_INT_FAST64_T 8

#define H5_SIZEOF_UINT_FAST8_T  1
#define H5_SIZEOF_UINT_FAST16_T 4
#define H5_SIZEOF_UINT_FAST32_T 4
#define H5_SIZEOF_UINT_FAST64_T 8

#define H5_SIZEOF_INT_LEAST8_T  1
#define H5_SIZEOF_INT_LEAST16_T 2
#define H5_SIZEOF_INT_LEAST32_T 4
#define H5_SIZEOF_INT_LEAST64_T 8

#define H5_SIZEOF_UINT_LEAST8_T  1
#define H5_SIZEOF_UINT_LEAST16_T 2
#define H5_SIZEOF_UINT_LEAST32_T 4
#define H5_SIZEOF_UINT_LEAST64_T 8

#if _MSC_VER >= 1900
#include <stdint.h>
#else
typedef char    int8_t;
typedef short   int16_t;
typedef int     int32_t;
typedef __int64 int64_t;

typedef unsigned char    uint8_t;
typedef unsigned short   uint16_t;
typedef unsigned int     uint32_t;
typedef unsigned __int64 uint64_t;

typedef int8_t   int_least8_t;
typedef int16_t  int_least16_t;
typedef int32_t  int_least32_t;
typedef int64_t  int_least64_t;

typedef uint8_t   uint_least8_t;
typedef uint16_t  uint_least16_t;
typedef uint32_t  uint_least32_t;
typedef uint64_t  uint_least64_t;

typedef int8_t   int_fast8_t;
typedef int32_t  int_fast16_t;
typedef int32_t  int_fast32_t;
typedef int64_t  int_fast64_t;

typedef uint8_t   uint_fast8_t;
typedef uint32_t  uint_fast16_t;
typedef uint32_t  uint_fast32_t;
typedef uint64_t  uint_fast64_t;
#endif

#ifdef _WIN64
#define H5_SIZEOF_SIZE_T      8
#define H5_SIZEOF_SSIZE_T     8
#define H5_PRINTF_LL_WIDTH    "ll" 
typedef __int64 ssize_t;
#else
#define H5_SIZEOF_SIZE_T      4
#define H5_SIZEOF_SSIZE_T     4
#define H5_PRINTF_LL_WIDTH    "l" 
typedef int ssize_t;
#endif

#define H5_HAVE___int64
#define H5_SIZEOF___INT64 8 

#define H5_HAVE_IO_H         1
#define H5_HAVE_LARGE_HSIZET 1 

#define H5_HAVE_WINSOCK_H 1
/*G.Barrand #define H5_HAVE_SZLIB_H 1 */
/*G.Barrand #define H5_HAVE_FILTER_SZIP 1 */
/*G.Barrand #define H5_SZIP_CAN_ENCODE 1 */
#if defined(__cplusplus)
#define H5_inline inline
#else
#define H5_inline __inline
#endif

#if _MSC_VER >= 1300 /* .Net supports FUNCTION */
#define H5_HAVE_FUNCTION 1
#else
#undef H5_HAVE_FUNCTION
#endif 

/* we do not have pthread.h. (See H5private.h). */
/*#define H5_HAVE_THREADSAFE 1*/   /*add H5TS.c*/

/* //////////////////////////////////////////////////////// */
/* /// CYGWIN and GNU : /////////////////////////////////// */
/* //////////////////////////////////////////////////////// */
#elif (defined(__CYGWIN__) && defined(__GNUC__))

#define H5_HAVE_UNISTD_H 1
#define H5_HAVE_STDINT_H 1

#define H5_HAVE_THREADSAFE 1   /*add H5TS.c*/

#if defined(__x86_64__)
/*#elif defined(_LP64) ??? */
#define H5_OUREX_64

#define H5_SIZEOF_INT_FAST16_T 8
#define H5_SIZEOF_INT_FAST32_T 8
#define H5_SIZEOF_UINT_FAST16_T 8
#define H5_SIZEOF_UINT_FAST32_T 8

#else
#define H5_OUREX_32
#endif

/* //////////////////////////////////////////////////////// */
/* /// EMSCRIPTEN : /////////////////////////////////////// */
/* //////////////////////////////////////////////////////// */
#elif defined(EMSCRIPTEN)

#define H5_OUREX_32
//#define H5_OUREX_64

#define H5_HAVE_UNISTD_H 1
#define H5_HAVE_STDINT_H 1

/* //////////////////////////////////////////////////////// */
/* /// unknown platform : ///////////////////////////////// */
/* //////////////////////////////////////////////////////// */
#else
#error "ourex/hdf5/include/H5pubconf.h : unknown platform."
#endif

/* //////////////////////////////////////////////////////// */
/* /// common : /////////////////////////////////////////// */
/* //////////////////////////////////////////////////////// */
#if defined(H5_OUREX_32)
#define H5_SIZEOF_LONG 4
#define H5_SIZEOF_LONG_DOUBLE 12
#define H5_SIZEOF_LONG_LONG 8
#define H5_SIZEOF_OFF_T 8
#define H5_SIZEOF_SIZE_T 4
#define H5_SIZEOF_SSIZE_T 4

#define H5_SIZEOF_INT8_T  1
#define H5_SIZEOF_INT16_T 2
#define H5_SIZEOF_INT32_T 4
#define H5_SIZEOF_INT64_T 8

#define H5_SIZEOF_UINT8_T  1
#define H5_SIZEOF_UINT16_T 2
#define H5_SIZEOF_UINT32_T 4
#define H5_SIZEOF_UINT64_T 8

#define H5_SIZEOF_INT_FAST8_T  1
#define H5_SIZEOF_INT_FAST16_T 4
#define H5_SIZEOF_INT_FAST32_T 4
#define H5_SIZEOF_INT_FAST64_T 8

#define H5_SIZEOF_UINT_FAST8_T  1
#define H5_SIZEOF_UINT_FAST16_T 4
#define H5_SIZEOF_UINT_FAST32_T 4
#define H5_SIZEOF_UINT_FAST64_T 8

#define H5_SIZEOF_INT_LEAST8_T  1
#define H5_SIZEOF_INT_LEAST16_T 2
#define H5_SIZEOF_INT_LEAST32_T 4
#define H5_SIZEOF_INT_LEAST64_T 8

#define H5_SIZEOF_UINT_LEAST8_T  1
#define H5_SIZEOF_UINT_LEAST16_T 2
#define H5_SIZEOF_UINT_LEAST32_T 4
#define H5_SIZEOF_UINT_LEAST64_T 8

#define H5_SIZEOF___INT64 0

#define H5_PRINTF_LL_WIDTH "ll"
#endif /*H5_OUREX_32*/

#if defined(H5_OUREX_64)
#define H5_SIZEOF_LONG 8
#define H5_SIZEOF_LONG_DOUBLE 16
#define H5_SIZEOF_LONG_LONG 8
#define H5_SIZEOF_OFF_T 8
#define H5_SIZEOF_SIZE_T 8
#define H5_SIZEOF_SSIZE_T 8

#define H5_SIZEOF_INT8_T  1
#define H5_SIZEOF_INT16_T 2
#define H5_SIZEOF_INT32_T 4
#define H5_SIZEOF_INT64_T 8

#define H5_SIZEOF_UINT8_T  1
#define H5_SIZEOF_UINT16_T 2
#define H5_SIZEOF_UINT32_T 4
#define H5_SIZEOF_UINT64_T 8

#define H5_SIZEOF_INT_FAST8_T  1
#define H5_SIZEOF_INT_FAST64_T 8

#define H5_SIZEOF_UINT_FAST8_T  1
#define H5_SIZEOF_UINT_FAST64_T 8

#define H5_SIZEOF_INT_LEAST8_T  1
#define H5_SIZEOF_INT_LEAST16_T 2
#define H5_SIZEOF_INT_LEAST32_T 4
#define H5_SIZEOF_INT_LEAST64_T 8

#define H5_SIZEOF_UINT_LEAST8_T  1
#define H5_SIZEOF_UINT_LEAST16_T 2
#define H5_SIZEOF_UINT_LEAST32_T 4
#define H5_SIZEOF_UINT_LEAST64_T 8

#define H5_SIZEOF___INT64 0

#define H5_PRINTF_LL_WIDTH "l"
#endif /*H5_OUREX_64*/

/* //////////////////////////////////////////////////////// */
/* /// Seems mendatory to compile : /////////////////////// */
/* //////////////////////////////////////////////////////// */

#define H5_STDC_HEADERS 1

#define H5_HAVE_SETJMP_H 1
#define H5_HAVE_STDDEF_H 1
#define H5_HAVE_STDLIB_H 1
#define H5_HAVE_STRING_H 1

#define H5_HAVE_SYS_STAT_H 1
#define H5_HAVE_SYS_TYPES_H 1

#define H5_HAVE_DIFFTIME 1

/* //////////////////////////////////////////////////////// */
/* /// options : ////////////////////////////////////////// */
/* //////////////////////////////////////////////////////// */
#define H5_HAVE_ZLIB_H 1

#define H5_HAVE_FILTER_DEFLATE 1
#define H5_HAVE_FILTER_FLETCHER32 1
#define H5_HAVE_FILTER_SHUFFLE 1

/*#define H5_HAVE_STREAM     1*/  /*add H5FDstream.c*/
