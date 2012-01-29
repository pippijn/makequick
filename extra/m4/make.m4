AC_DEFUN([_AP_MAKE_RUN],[
   cat <<'EOF' > autotest-$$.mk
$1
EOF
   ${MAKE-make} -f autotest-$$.mk foo.outext > autotest-$$.out 2>&1
   $2

   rm -f foo.1 foo.2 foo.outext
   rm -f autotest-$$.mk autotest-$$.out
])
AC_DEFUN([_AP_MAKE_ALLSRC],[
   if test -z "$ALLSRC"; then
      _AP_MAKE_RUN([
foo.1: ; touch $[@]
foo.2: ; touch $[@]
foo.outext: foo.1 foo.2 ; echo "A ${$1} B"
], [
      if grep "A foo.1 foo.2 B" autotest-$$.out > /dev/null; then
         ALLSRC='{$1}'
      fi
   ])
   fi
])

AC_DEFUN([AP_PROG_MAKE],[
   AC_MSG_CHECKING([whether ${MAKE-make} supports GNU-style pattern rules])
   _AP_MAKE_RUN([
%.1: ; touch $[@]
%.outext: %.1 ; echo OK
], [
      if test x$? = x0; then
         AC_MSG_RESULT(yes)
      else
         AC_MSG_RESULT(no)
         IF_PATRULES='#'
      fi
      AC_SUBST([IF_PATRULES])
   ])

   AC_MSG_CHECKING([whether ${MAKE-make} sets \${RM}])
   touch foo.ext
   _AP_MAKE_RUN([
foo.outext: ; $(RM) foo.ext
], [
      if test -f foo.ext; then
         AC_MSG_RESULT(no)
         rm -f foo.ext
         SET_RM="RM=${RM-rm -f}"
      else
         AC_MSG_RESULT(yes)
      fi
      AC_SUBST([SET_RM])
   ])


   AC_MSG_CHECKING([what ${MAKE-make} calls the .ALLSRC variable])
   dnl GNU Make
   _AP_MAKE_ALLSRC([+])
   dnl NetBSD Make
   _AP_MAKE_ALLSRC([>])
   _AP_MAKE_ALLSRC([.ALLSRC])
   if test -z "$ALLSRC"; then
      AC_MSG_RESULT([unknown])
      AC_MSG_ERROR([could not determine the spelling of the .ALLSRC variable; please report to $PACKAGE_BUGREPORT])
   else
      AC_MSG_RESULT([\$$ALLSRC])
      AC_SUBST([ALLSRC])
   fi
])
