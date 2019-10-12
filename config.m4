dnl config.m4 for extension xinject

PHP_ARG_ENABLE(xinject, whether to enable xinject support,
[  --enable-xinject        Enable xinject support])

if test "$PHP_XINJECT" != "no"; then

  PHP_XINJECT_CFLAGS="$STD_CFLAGS $MAINTAINER_CFLAGS"

  PHP_NEW_EXTENSION(xdebug, xinject.c, $ext_shared,,$PHP_XINJECT_CFLAGS,,yes)
  PHP_SUBST(XINJECT_SHARED_LIBADD)
  PHP_ADD_MAKEFILE_FRAGMENT
fi
