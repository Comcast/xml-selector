TESTS = check_search check_xq

check_PROGRAMS = check_search check_xq

check_search_SOURCES = check_search.c $(top_builddir)/libxq.h
check_search_CFLAGS = @CHECK_CFLAGS@ @LIBXML_CFLAGS@
check_search_LDFLAGS = @LIBXML_LFLAGS@
check_search_LDADD = $(top_builddir)/libxq.la @CHECK_LIBS@

check_xq_SOURCES = check_xq.c $(top_builddir)/libxq.h
check_xq_CFLAGS = @CHECK_CFLAGS@ @LIBXML_CFLAGS@
check_xq_LDFLAGS = @LIBXML_LFLAGS@
check_xq_LDADD = $(top_builddir)/libxq.la @CHECK_LIBS@
