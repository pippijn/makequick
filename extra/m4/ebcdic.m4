dnl Copyright © 2010 Pippijn van Steenhoven
dnl You can redistribute this file under the terms of the
dnl GNU Affero General Public License version 3 or later.

AC_DEFUN([AP_CHECK_CHARSET],[

	AC_MSG_CHECKING([if your character set is EBCDIC])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM([
			dnl We add extra quotes for the [] inside.
			[typedef char check['0' == 240 ? 1 : -1];]
		])],
		[AC_DEFINE(
			[HAVE_EBCDIC],
			[1],
			[Define this to 1 if your character set is EBCDIC.])
		 AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)
		 AC_MSG_CHECKING([if your character set is ASCII])
		 AC_COMPILE_IFELSE(
			 [AC_LANG_PROGRAM([
				 dnl We add extra quotes for the [] inside.
				 [typedef char check['0' == 48 ? 1 : -1];]
			 ])],
			 [AC_MSG_RESULT(yes)],
			 [AC_MSG_ERROR(unknown character set)]
		 )
		]
	)

])
