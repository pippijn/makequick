AC_DEFUN([AP_PROG_PERL], [
	AC_PATH_PROG(PERL, perl5.10.1)
	if test -z "$PERL"; then
		AC_PATH_PROG(PERL, perl5.10.0)
		if test -z "$PERL"; then
			AC_PATH_PROG(PERL, perl)
			if test -z "$PERL"; then
				AC_MSG_ERROR([perl 5.10 or higher must be in your PATH, check the README])
			fi
		fi
	fi
])

AC_DEFUN([AP_PERL_MODS], [
	AC_REQUIRE([AP_PROG_PERL])
	for module_vers in	\
		$1						\
	; do
		set -- $module_vers
		module=${1}
		minvers=${2}
		AC_MSG_CHECKING([for $module perl module version $minvers or higher])
		if $PERL -M$module -e0 >/dev/null 2>&1; then
			version=`$PERL -M$module -e "print \\$$module::VERSION"` #''
			if $PERL -M$module -e "\$$module::VERSION >= $minvers or die" > /dev/null 2>&1; then
				AC_MSG_RESULT([ok, version $version])
			else
				AC_MSG_ERROR([no, installed version is $version])
			fi
		else
			AC_MSG_ERROR([no, cannot load module $module])
		fi
	done
])

AC_DEFUN([AP_LIB_PERL], [
	AC_REQUIRE([AP_PROG_PERL])
	AC_MSG_CHECKING(for $PERL suitability)
	if $PERL -MExtUtils::Embed -e "use 5.10.0" >/dev/null 2>&1; then
		save_CFLAGS="$CFLAGS"
		save_LIBS="$LIBS"

		xPERLFLAGS="`$PERL -MExtUtils::Embed -e ccopts`" #''
		xPERLLIB="`$PERL -MExtUtils::Embed -e ldopts`" #''

		CFLAGS="$CFLAGS $xPERLFLAGS"
		LIBS="$LIBS $xPERLLIB"

		AC_LANG_PUSH([C])
		AC_LINK_IFELSE([AC_LANG_PROGRAM([[
			#include <EXTERN.h>
			#include <perl.h>
			#include <XSUB.h>
		]], [[
			PerlInterpreter *perl = perl_alloc ();
		]])],[perl_link=yes],[perl_link=no])
		AC_LANG_POP([C])

		CFLAGS="$save_CFLAGS"
		LIBS="$save_LIBS"

		if test x$perl_link = xyes; then
			AC_MSG_RESULT(ok)
			PERLFLAGS="$xPERLFLAGS"
			PERLLIB="$xPERLLIB"
			PERLPRIVLIBEXP="`$PERL -MConfig -e 'print $Config{privlibexp}'`" #'
			CPPFLAGS="$CPPFLAGS $PERLFLAGS"
		else
			AC_MSG_ERROR([no, unable to link test program])
		fi
	else
		AC_MSG_ERROR([no working perl found, or perl not version >= 5.10])
	fi
	AC_SUBST(PERLLIB)
	AC_SUBST(PERLFLAGS)
	AC_SUBST(PERLPRIVLIBEXP)
	AC_SUBST(PERL)
])
