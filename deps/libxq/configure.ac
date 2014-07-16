AC_INIT([xq], [1.0], [jeff_hunter@sv.comcast.com])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AR
AM_PATH_CHECK

LT_INIT

XQ_SETUP_LIBXML([
  AC_MSG_ERROR([xml2-config not found. Please check your libxml2 installation.])
])

AC_C_INLINE

if test "$ac_cv_c_inline" != no ; then
  AC_DEFINE([HAVE_INLINE],[1],[Defined to 1 if the C compiler supports the inline keyword])
  AC_SUBST([HAVE_INLINE])
fi

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile tests/Makefile])

AC_OUTPUT
