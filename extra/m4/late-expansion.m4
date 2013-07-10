dnl Copyright © 2010 Pippijn van Steenhoven
dnl You can redistribute this file under the terms of the
dnl GNU Affero General Public License version 3 or later.

AC_DEFUN([AP_CHECK_LATE_EXPANSION],[

	AC_MSG_CHECKING([for ANSI conforming late macro expansion])
	AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([
			static int MACRO = 0;
			static int abc   = 1;

			#define EXPAND1(a) a
			#define EXPAND2(a) EXPAND1(a)
			#define MACRO abc
		],[
			return EXPAND1 (MACRO) == 0 && EXPAND2 (MACRO) == 1;
		])],
		[AC_DEFINE(
			[HAVE_LATE_EXPANSION],
			[1],
			[Define this to 1 if your compiler has late macro expansion.])
		 AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)

])
