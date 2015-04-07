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
{
  "targets": [
    {
      "target_name": "xml2",
      "product_prefix": "lib",
      "type": "static_library",
      "include_dirs": [
        "libxml2/include",
        "xmlconfig"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "libxml2/include"
        ]
      },
      "sources": [
        "libxml2/SAX.c",
        "libxml2/entities.c",
        "libxml2/encoding.c",
        "libxml2/error.c",
        "libxml2/parserInternals.c",
        "libxml2/parser.c",
        "libxml2/tree.c",
        "libxml2/hash.c",
        "libxml2/list.c",
        "libxml2/xmlIO.c",
        "libxml2/xmlmemory.c",
        "libxml2/uri.c",
        "libxml2/valid.c",
        "libxml2/xlink.c",
        "libxml2/HTMLparser.c",
        "libxml2/HTMLtree.c",
        "libxml2/debugXML.c", # not in previous lib
        "libxml2/xpath.c",
        "libxml2/xpointer.c",
        "libxml2/xinclude.c",
        "libxml2/nanohttp.c", # not in previous lib
        "libxml2/nanoftp.c", # not in previous lib
        "libxml2/catalog.c",
        "libxml2/globals.c",
        "libxml2/threads.c",
        "libxml2/c14n.c", # not in previous lib
        "libxml2/xmlstring.c",
        "libxml2/buf.c",
        "libxml2/xmlregexp.c",
        "libxml2/xmlschemas.c",
        "libxml2/xmlschemastypes.c",
        "libxml2/xmlunicode.c",
        "libxml2/xmlreader.c",
        "libxml2/relaxng.c",
        "libxml2/dict.c",
        "libxml2/SAX2.c",
        "libxml2/xmlwriter.c",
        "libxml2/legacy.c",
        "libxml2/chvalid.c",
        "libxml2/pattern.c",
        "libxml2/xmlsave.c",
        "libxml2/xmlmodule.c",
        "libxml2/schematron.c", # not in previous lib
        "libxml2/xzlib.c" # not in previous lib
      ]
    }
  ]
}