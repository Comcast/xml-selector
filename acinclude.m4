dnl 
dnl XQ_SETUP_LIBXML([action-not-found])
dnl
dnl Setup macro for libxml
dnl
AC_DEFUN([XQ_SETUP_LIBXML], [
AC_CACHE_CHECK([for xml2-config path], ac_cv_xq_xml2_config_path,
[
  for i in $XQ_LIBXML_DIR /usr/local /usr; do
    if test -x "$i/bin/xml2-config"; then
      ac_cv_xq_xml2_config_path="$i/bin/xml2-config"
      break
    fi
  done
])

  if test -x "$ac_cv_xq_xml2_config_path"; then
    XML2_CONFIG="$ac_cv_xq_xml2_config_path"
    libxml_full_version=`$XML2_CONFIG --version`
    ac_IFS=$IFS
    IFS="."
    set $libxml_full_version
    IFS=$ac_IFS
    LIBXML_VERSION=`expr [$]1 \* 1000000 + [$]2 \* 1000 + [$]3`
    if test "$LIBXML_VERSION" -ge "2006011"; then
      LIBXML_LIBS=`$XML2_CONFIG --libs`
      LIBXML_INCS=`$XML2_CONFIG --cflags`
      AC_SUBST([LIBXML_CFLAGS],[$LIBXML_INCS])
      AC_SUBST([LIBXML_LFLAGS],[$LIBXML_LIBS])
    else
      AC_MSG_ERROR([libxml2 version 2.6.11 or greater required.])
    fi
ifelse([$1],[],,[else $1])
  fi
])
