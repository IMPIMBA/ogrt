#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([OGRT], [devel], [rath.georg@gmail.com])
AC_CONFIG_SRCDIR([src/ogrt-readso.c])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_INIT

# Checks for programs.
AC_PROG_CC

AX_CHECK_COMPILE_FLAG([-std=c11], [CFLAGS+=" -std=c11"], [
  echo "C compiler cannot compile C11 code"
  exit -1
])

AC_SEARCH_LIBS([dlsym], [dl], [], [
  AC_MSG_ERROR([unable to find the dlopen() function])
])

AC_SEARCH_LIBS([clock_gettime], [rt], [], [
  AC_MSG_ERROR([unable to find the clock_gettime() function])
])

PKG_CHECK_MODULES([protobufc], [libprotobuf-c])
PKG_CHECK_MODULES([uuid], [uuid])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netdb.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([clock_gettime gethostname memset realpath socket strchr strdup])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
