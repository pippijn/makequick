dnl Copyright © 2010 Pippijn van Steenhoven
dnl You can redistribute this file under the terms of the
dnl GNU Affero General Public License version 3 or later.

AC_DEFUN([AP_CHECK_TOKEN_PASTE],[

	AC_MSG_CHECKING([for ANSI-style token pasting])
	AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([
			#define PASTE1(a, b) a ## b
			#define PASTE2(a, b) a## b
			#define PASTE3(a, b) a ##b
			#define PASTE4(a, b) a##b
		],[
			int ab = 0;
			return PASTE1 (a, b)
				  + PASTE2 (a, b)
				  + PASTE3 (a, b)
				  + PASTE4 (a, b)
				  ;
		])],
		[AC_DEFINE(
			[HAVE_ANSI_PASTE],
			[1],
			[Define this to 1 if your compiler implements ANSI-style token pasting.])
		 AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)

])
