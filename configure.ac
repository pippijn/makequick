AC_PREREQ([2.65])
AC_INIT([MakePP],[0.1],[makepp@xinutec.org])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_AUX_DIR(autoconf)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS(config.h)

AP_STATUS([automake support])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([silent-rules gnits color-tests parallel-tests])
AM_SILENT_RULES([yes])

AC_LANG(C++)

AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_LEX
AC_PROG_YACC

AP_PKG_CHECK([protobuf],[2.4.0])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
