dnl $Id$
dnl config.m4 for extension sixplz

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(sixplz, for sixplz support,
dnl Make sure that the comment is aligned:
dnl [  --with-sixplz             Include sixplz support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(sixplz, whether to enable sixplz support,
Make sure that the comment is aligned:
[  --enable-sixplz           Enable sixplz support])

if test "$PHP_SIXPLZ" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-sixplz -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/sixplz.h"  # you most likely want to change this
  dnl if test -r $PHP_SIXPLZ/$SEARCH_FOR; then # path given as parameter
  dnl   SIXPLZ_DIR=$PHP_SIXPLZ
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for sixplz files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       SIXPLZ_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$SIXPLZ_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the sixplz distribution])
  dnl fi

  dnl # --with-sixplz -> add include path
  dnl PHP_ADD_INCLUDE($SIXPLZ_DIR/include)

  dnl # --with-sixplz -> check for lib and symbol presence
  dnl LIBNAME=sixplz # you may want to change this
  dnl LIBSYMBOL=sixplz # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $SIXPLZ_DIR/lib, SIXPLZ_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_SIXPLZLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong sixplz lib version or lib not found])
  dnl ],[
  dnl   -L$SIXPLZ_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(SIXPLZ_SHARED_LIBADD)

  sixplz_sources="libsixplz/fastlz.c\
  libsixplz/6pack.c\
  libsixplz/6unpack.c\
  libsixplz/6pack_common.c\
  sixplz.c"

  PHP_NEW_EXTENSION(sixplz, $sixplz_sources, $ext_shared)
  PHP_ADD_BUILD_DIR([$ext_builddir/libsixplz])
fi
