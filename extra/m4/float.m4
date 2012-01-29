dnl Check for floating-point format and double precision word order.
dnl Just as for integers, the bytes in a word can be small of big endian.
dnl There is already a standard autoconf macro (AC_C_BIGENDIAN) that you 
dnl should use to check this for integers - I have never heard of a machine
dnl where it is not the same for integer and fp variables, but we still check
dnl it separately for fp variables here to be sure.
dnl
dnl However, in double precision there are also two ways to arrange the words
dnl forming a double (8-byte=2-word) variable.
dnl Normally this order is the same as the endian, but there are 
dnl exceptions (e.g. ARM)
dnl We detect it by compiling a small test program and grepping into it.
dnl
AC_DEFUN([AP_FLOAT_FORMAT],[
   AC_CACHE_CHECK(floating-point format, ac_cv_float_format,[
      cat >conftest.$ac_ext <<EOF
      [/* Check that a double is 8 bytes - die if it isnt */
      extern char xyz [sizeof(double) == 8 ? 1 : -1];
      double abc [] = {
        /* "GROMACSX" in ascii    */
        (double)  3.80279098314984902657e+35 , 
        /* "GROMACSX" in ebcdic   */
        (double) -1.37384666579378297437e+38 , 
        /* "D__float" (vax)       */
        (double)  3.53802595280598432000e+18 , 
        /* "IBMHEXFP" s390/ascii  */
        (double)  1.77977764695171661377e+10 , 
        /* "IBMHEXFP" s390/ebcdic */
        (double) -5.22995989424860458374e+10 };
      ]
EOF
      if AC_TRY_EVAL(ac_compile); then
         # dont match first and last letter because of rounding errors.
         # next: big-endian - string is GROMACSX 
         if   grep 'ROMACS' conftest.o >/dev/null 2>&1; then
            ac_cv_float_format='IEEE754 (big-endian byte and word order)'
         # next: big-endian byte order, but little-endian word order - ACSXGROM
         elif grep 'CSXGRO' conftest.o >/dev/null 2>&1; then
            ac_cv_float_format='IEEE754 (big-endian byte, little-endian word order)'
         # next: little-endian - XSCAMORG
         elif grep 'SCAMOR' conftest.o >/dev/null 2>&1; then
            ac_cv_float_format='IEEE754 (little-endian byte and word order)'
         # next: little-endian byte order, but big-endian word order - MORGXSCA
         elif grep 'ORGXSC' conftest.o >/dev/null 2>&1; then
            ac_cv_float_format='IEEE754 (big-endian byte, little-endian word order)'
         elif grep '__floa' conftest.o >/dev/null 2>&1; then
            ac_cv_float_format='VAX D-float'
         elif grep 'BMHEXF' conftest.o >/dev/null 2>&1; then
            ac_cv_float_format='IBM 370 hex'
         else
            AC_MSG_WARN([Unknown floating-point format])
         fi
      else
         AC_MSG_ERROR(compile failed)
      fi
      rm -rf conftest*
   ])
   case $ac_cv_float_format in
      'IEEE754 (big-endian byte and word order)')
         format=IEEE754
         byteorder=big
         wordorder=big            
         ;;
      'IEEE754 (little-endian byte and word order)')
         format=IEEE754
         byteorder=little
         wordorder=little
         ;;
      'IEEE754 (big-endian byte, little-endian word order)')
         format=IEEE754
         byteorder=big
         wordorder=little
         ;;
      'IEEE754 (litte-endian byte, big-endian word order)')
         format=IEEE754
         byteorder=little
         wordorder=big            
         ;;
      'VAX D-float')
         AC_DEFINE(FLOAT_FORMAT_VAX, [1], [VAX floating-point format if set])
         ;;
      'IBM 370 hex')
         AC_DEFINE(FLOAT_FORMAT_IBM_HEX, [1], [IBM HEX floating-point format if set (s390?)])
         ;;   
      *)
         format=Unknown   
         ;;
   esac
   if test "$format" = "IEEE754"; then
      AC_DEFINE(FLOAT_FORMAT_IEEE754, [1], [IEEE754 floating-point format. Memory layout is defined by
				macros IEEE754_BIG_ENDIAN_BYTE_ORDER and IEEE754_BIG_ENDIAN_WORD_ORDER.])
   fi
   if test "$byteorder" = "big"; then
      AC_DEFINE(IEEE754_BIG_ENDIAN_BYTE_ORDER, [1], [Bytes in IEEE fp word are in big-endian order if set,
				little-endian if not. Only relevant when FLOAT_FORMAT_IEEE754 is defined.])
   fi
   if test "$wordorder" = "big"; then
      AC_DEFINE(IEEE754_BIG_ENDIAN_WORD_ORDER, [1], [The two words in a double precision variable are in
				big-endian order if set, little-endian if not. Do NOT assume this is the same as the byte order! 
				Only relevant when FLOAT_FORMAT_IEEE754 is defined.])
   fi
])
