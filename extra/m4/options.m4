AC_DEFUN([AP_ADD_FLAGS], [
	$2="${$2} $1"
])

dnl AP_ARG_ENABLE(var, [msg], [action], #define, default)
AC_DEFUN([AP_ARG_ENABLE],[
	AC_ARG_ENABLE($1,
		AS_HELP_STRING([--enable-$1], $2),
		[patsubst($1,-,_)_enabled=no
		 if test x$enableval = xyes; then
			 patsubst($1,-,_)_enabled=yes
		 fi],
		[patsubst($1,-,_)_enabled=$4]
	)
	m4_ifval($3, [
		if test x$patsubst($1,-,_)_enabled = xyes; then
			$3
		fi
	])
	AM_CONDITIONAL(translit($1,[a-z-],[A-Z_]), test x$patsubst($1,-,_)_enabled = xyes)
	if test x$patsubst($1,-,_)_enabled = xyes; then
		AC_SUBST(translit($1,[a-z-],[A-Z_]), 1)
		m4_ifval($5, [AC_DEFINE($5, 1, $2)], [])
	else
		AC_SUBST(translit($1,[a-z-],[A-Z_]), 0)
		m4_ifval($5, [AC_DEFINE($5, 0, $2)], [])
	fi
])

AC_DEFUN([_AP_ARG_FORCE],[
	$1_enabled=$2
	if test x$2 = xyes; then
		$3_TRUE=
		$3_FALSE=#
		$3=1
	else
		$3_TRUE=#
		$3_FALSE=
		$3=0
	fi
])

AC_DEFUN([AP_ARG_FORCE],[
	_AP_ARG_FORCE($1, $2, translit($1,[a-z-],[A-Z_]))
])
