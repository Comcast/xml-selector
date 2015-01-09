#
# Copyright 2013-2015 Comcast Cable Communications Management, LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
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
