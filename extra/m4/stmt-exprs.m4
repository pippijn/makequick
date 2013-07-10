dnl Copyright © 2010 Pippijn van Steenhoven
dnl You can redistribute this file under the terms of the
dnl GNU Affero General Public License version 3 or later.

AC_DEFUN([AP_CHECK_STMT_EXPRS],

	AC_MSG_CHECKING([for support for brace-expressions])
	AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([],[
			for (;  __extension__ ({ do { } while (0); 0; }); ) ;
			for (;; __extension__ ({ do { } while (0); 0; })  ) ;
			return __extension__ ({ 0; });
		])],
		[AC_DEFINE(
			[HAVE_STMT_EXPRS],
			[1],
			[Define this to 1 if your compiler supports braced statements within expressions.])
		 AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)

])
