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
SUBDIRS = . tests

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES= libxq.la
libxq_la_SOURCES = nodelist.c xq.c search.c traverse.c
libxq_la_CFLAGS = @LIBXML_CFLAGS@
libxq_la_LDFLAGS = @LIBXML_LFLAGS@

include_HEADERS = libxq.h
