dnl Copyright © 2010 Pippijn van Steenhoven
dnl You can redistribute this file under the terms of the
dnl GNU Affero General Public License version 3 or later.

AC_DEFUN([AP_C_VARIADIC_TEMPLATES],[

	AC_MSG_CHECKING([for lambda expression support])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM([[
			auto lambda = [](char const* msg) { };
			void test () {
				lambda ("Hello");
			}
		]])],
		[AC_DEFINE([HAVE_LAMBDA], 1, [Define if your compiler supports C++ lambda expressions])
		 AC_MSG_RESULT([yes])],
		[AC_MSG_RESULT([no])]
	)

])
