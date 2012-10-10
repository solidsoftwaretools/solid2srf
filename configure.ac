#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.64)
AC_INIT(solid2srf, 0.7.6, [http://solidsoftwaretools.com/])
AM_CONFIG_HEADER(srf_config.h)
AM_INIT_AUTOMAKE([foreign])

dnl AX_CFLAGS_WARN_ALL_ANSI
dnl FLAGS must be set before CC and CXX macros
dnl This respects user settings, but provides a good default if none specified
test_CFLAGS=${CFLAGS+set}
if test "$test_CFLAGS" != set; then
    CFLAGS="-g -O2 -Wall -Wstrict-aliasing=2"
fi
test_CXXFLAGS=${CXXFLAGS+set}
if test "$test_CXXFLAGS" != set; then
    CXXFLAGS="$CFLAGS $CXXFLAGS"
fi
export CFLAGS CXXFLAGS

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB

dnl AC_ENABLE_SHARED

dnl Checks for libraries

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([inttypes.h fcntl.h limits.h unistd.h stddef.h stdlib.h string.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADERS([endian.h sys/endian.h mach/endian.h machine/endian.h])
AC_C_BIGENDIAN
AC_CHECK_SIZEOF(short,2)
AC_CHECK_SIZEOF(int,4)
AC_CHECK_SIZEOF(long,4)
AC_C_INLINE
AC_C_CONST
AC_TYPE_MODE_T
dnl AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL

dnl AC_NEED_STDINT_H
AC_CHECK_TYPES([int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t])
dnl AC_TYPE_INT32_T
dnl AC_TYPE_INT64_T
AC_TYPE_MODE_T
AC_TYPE_SIZE_T
dnl AC_TYPE_UINT16_T
dnl AC_TYPE_UINT32_T
dnl AC_TYPE_UINT64_T
dnl AC_TYPE_UINT8_T

dnl Large file support
AC_SYS_LARGEFILE
AC_FUNC_FSEEKO

dnl Checks for library functions.
AC_FUNC_STAT					
AC_CHECK_FUNCS([memset strdup])
AC_SEARCH_LIBS(cos, m)
dnl AC_FUNC_STRFTIME
dnl AC_FUNC_VPRINTF
dnl AC_CHECK_FUNCS(mktime strspn strstr strtol)

AC_CACHE_CHECK([if printf confirms to C99 for 0.0001], [ac_cv_bug_printf],
AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
#include <string.h>
char buf[11];
]],
[[
   snprintf(buf,10,"%0.6g", 0.0001f);
   return(strncmp(buf, "0.0001", 10) == 0);
]])],
   [ac_cv_bug_printf=broken],
   [ac_cv_bug_printf=ok],
   [ac_cv_bug_printf=skip]))
if test $ac_cv_bug_printf = broken; then
    AC_DEFINE(BROKEN_PRINTF, 1, "Defined if printf outputs 1e-4 when given 0.0001") ]
fi


dnl Check for zlib
dnl AC_CHECK_LIB(z, gzopen, [AC_CHECK_HEADERS(zlib.h,, AC_MSG_ERROR(ZLIB headers not found))], [AC_MSG_ERROR(ZLIB library not found)])
#AC_ARG_ENABLE(zlib, [  --enable-zlib           turn on ZLIB support: auto,no (default=auto)])
#if test x$enable_zlib != xno; then
#    AC_CHECK_HEADER(zlib.h,[ AC_CHECK_LIB(z, gzopen,[
#     AC_DEFINE(HAVE_LIBZ, 1, "Have ZLIB")
#    ])])
#fi

dnl GCC extension for wrapping a pipe in a stream
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([ext/stdio_filebuf.h],, [AC_MSG_ERROR([__gnu_cxx::stdio_filebuf (GNU libstdc++) is not installed])])
AC_LANG_POP([C++])

dnl AC_CHECK_LIB(z, gzopen, [AC_CHECK_HEADERS(zlib.h,, AC_MSG_ERROR(ZLIB headers not found))], [AC_MSG_ERROR(ZLIB library not found)])

dnl AC_CHECK_LIB(curl, curl_easy_init, ,AC_MSG_ERROR([*** zlib missing - please install first or check config.log ***]))
dnl AC_CHECK_HEADER([curl.h], ,AC_MSG_ERROR([*** zlib.h missing - please install first or check config.log ***]))
 
AC_CONFIG_FILES([Makefile 
		         ZTR/Makefile
				 ZTR/src/Makefile
				 SRF/Makefile
				 SRF/base/Makefile
				 SRF/writer/Makefile
				 SRF/reader/Makefile])
AC_OUTPUT
