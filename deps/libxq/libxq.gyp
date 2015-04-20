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
      "target_name": "xq",
      "product_prefix": "lib",
      "type": "static_library",
      "include_dirs": [
        "./node"
      ],
      "direct_dependent_settings": {
        "include_dirs": [
          "./node"
        ]
      },
      "sources": [
        "nodelist.c",
        "xq.c",
        "search.c",
        "traverse.c"
      ],
      "dependencies": [
        "../libxml2.gyp:xml2"
      ]
    }
  ]
}