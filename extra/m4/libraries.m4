dnl This macro checks for the existence of a symbol in a library.
dnl
dnl AP_CHECK_LIB(library, symbol, header, [message-if-unavailable])
AC_DEFUN([AP_CHECK_LIB], [
   AH_CHECK_LIB([$1])
   
   AS_LITERAL_IF([$1],
      [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1_$2])],
      [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1''_$2])])
   
   AC_CACHE_CHECK([for $2 in -l$1], [ac_Lib],
      [ac_check_lib_save_LIBS=$LIBS
       LIBS="-l$1 $LIBS"
       
       AC_LINK_IFELSE(
          [AC_LANG_SOURCE([
             #include <$3>
             int main (void) {
                (void)$2;
             }
          ])],
          [AS_VAR_SET([ac_Lib], [yes])],
          [AS_VAR_SET([ac_Lib], [no])]
       )
       LIBS=$ac_check_lib_save_LIBS
      ]
   )

   AS_IF([test AS_VAR_GET([ac_Lib]) = yes],
      [lib$1_available=yes
       AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_LIB$1))
       AS_TR_CPP($1)_LIBS="-l$1"
      ],
      [lib$1_available=no
       m4_ifval([$5], [AC_MSG_ERROR([$5, but lib$1 or its header ($3) are not available])])
      ]
   )
   AS_VAR_POPDEF([ac_Lib])

   AC_SUBST(AS_TR_CPP($1)_LIBS)
])
