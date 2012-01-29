AC_DEFUN([AP_SUMMARY_START], [
   echo
   echo "Summary for $PACKAGE_STRING"
   echo "-------------------------------------------------------------------------------"
])

AC_DEFUN([AP_SUMMARY_LIST], [
   m4_foreach([x], $@, [
      printf "  %-60s %s\n" "m4_cdr(x)" "m4_car(x)"
   ])
])

AC_DEFUN([AP_SUMMARY_END], [
   echo "-------------------------------------------------------------------------------"
   echo
])
