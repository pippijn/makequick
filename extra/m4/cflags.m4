dnl AP_C_TRY_FLAGS(<warnings>,<cachevar>,<target>)
AC_DEFUN([AP_C_TRY_FLAGS],[
   AC_MSG_CHECKING([C compiler flag(s) $1])
   AC_CACHE_VAL($2,[
      oldcflags="${CFLAGS-}"
      oldldflags="${LDFLAGS-}"
      CFLAGS="${CFLAGS-} ${$3} $1 -Werror"
      LDFLAGS="${LDFLAGS-} -shared $lt_prog_compiler_pic"
      AC_LINK_IFELSE([
         AC_LANG_PROGRAM([
            #include <string.h>
            #include <stdio.h>
            int main (void);
         ],[
            if (strcmp ("a", "b"))
              fprintf (stdout, "test ok\n");
         ])],
         [$2=yes], [$2=no])
      LDFLAGS="${oldldflags}"
      CFLAGS="${oldcflags}"])
   if test "x$$2" = xyes; then
      $3="${$3} $1"
      AC_MSG_RESULT(ok)  
   else
      $2=''
      AC_MSG_RESULT(no)
   fi
])
