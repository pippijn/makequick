AC_DEFUN([_AP_PKG_CONFIG],[
   AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

   if test x$PKG_CONFIG = xno; then
      AC_MSG_ERROR([Could not find pkg-config])
   fi
])

AC_DEFUN([_AP_PKG_CHECK],[
   AC_REQUIRE([_AP_PKG_CONFIG])
   AC_MSG_CHECKING($1 >= $2)
   if $PKG_CONFIG $1 --atleast-version $2; then
      $3_CFLAGS="$$3_CFLAGS `$PKG_CONFIG $1 --cflags`" #''
      $3_LIBS="$$3_LIBS `$PKG_CONFIG $1 --libs`" #''
      AC_SUBST([$3_CFLAGS])
      AC_SUBST([$3_LIBS])
      AC_MSG_RESULT(ok)
   else
      AC_MSG_ERROR(no)
   fi
])

AC_DEFUN([AP_PKG_CHECK],[
   _AP_PKG_CHECK($1, $2, translit($1,[a-z.-],[A-Z__]))
])
