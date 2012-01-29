AC_DEFUN([AP_CHECK_MUDFLAP], [
   dnl Check for mudflap pointer debugging
   mudflap_enabled=no
   AP_ARG_ENABLE(mudflap,
      [Enable mudflap pointer debugging.]
      [], [], no)

   if test x$mudflap_enabled = xyes; then
      AC_CHECK_LIB([mudflap], [__wrap_main])
      AP_C_TRY_FLAGS([-fmudflap], ac_cv_f_mudflap, CFLAGS)
      AC_CHECK_HEADER([mf-runtime.h])
   fi
])
