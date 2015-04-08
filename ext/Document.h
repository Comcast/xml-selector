/**
 * Copyright 2013-2015 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __XMLSELECTOR_DOCUMENT_H_INCLUDED__
#define __XMLSELECTOR_DOCUMENT_H_INCLUDED__

#include <node.h>
#include <nan.h>

#include <libxml/tree.h>

namespace xmlselector {

class Document : public node::ObjectWrap {
public:
  static void Init(v8::Handle<v8::Object> exports);

  static v8::Persistent<v8::Function> constructor;

protected:

  explicit Document(xmlDocPtr doc);
  virtual ~Document();
  
  static NAN_METHOD(New);
  static NAN_METHOD(ParseFromString);
  
  xmlDocPtr _doc;
};

} // namespace xmlselector

#endif // __XMLSELECTOR_DOCUMENT_H_INCLUDED__