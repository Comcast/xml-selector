TESTS = check_search

check_PROGRAMS = check_search
check_search_SOURCES = check_search.c $(top_builddir)/libxq.h
check_search_CFLAGS = @CHECK_CFLAGS@ @LIBXML_CFLAGS@
check_search_LDFLAGS = @LIBXML_LFLAGS@
check_search_LDADD = $(top_builddir)/libxq.la @CHECK_LIBS@
