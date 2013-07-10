dnl Copyright © 2010 Pippijn van Steenhoven
dnl You can redistribute this file under the terms of the
dnl GNU Affero General Public License version 3 or later.

AC_DEFUN([AP_CHECK_ENUM_FWDECL],[

	AC_MSG_CHECKING([for ability to typedef enums before definition])
	AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([
			typedef enum fooEnum FooEnum;
			enum fooEnum {
			  FOO1,
			  FOO2
			};
		])],
		[AC_DEFINE(
			[HAVE_ENUM_FWDECL],
			[1],
			[Define this to 1 if your compiler supports `typedef enum' before `enum'.])
		 AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)]
	)

])
