dnl
dnl Copyright 2013-2015 Comcast Cable Communications Management, LLC
dnl
dnl Licensed under the Apache License, Version 2.0 (the "License");
dnl you may not use this file except in compliance with the License.
dnl You may obtain a copy of the License at
dnl
dnl http://www.apache.org/licenses/LICENSE-2.0
dnl
dnl Unless required by applicable law or agreed to in writing, software
dnl distributed under the License is distributed on an "AS IS" BASIS,
dnl WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl See the License for the specific language governing permissions and
dnl limitations under the License.

AC_INIT([xq], [1.0], [jeffrey_hunter@cable.comcast.com])
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
