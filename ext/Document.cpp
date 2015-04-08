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
#include <libxml/parser.h>

#include "Document.h"
#include "utils.h"

namespace xmlselector {


v8::Persistent<v8::Function> Document::constructor;

/**
 * Class initialization and exports
 */
void Document::Init(v8::Handle<v8::Object> exports) {
  // create a constructor function
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);

  tpl->SetClassName(NanNew<v8::String>("Document"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  // inherits from Node
  tpl->Inherit(NanNew(Node::constructor_template));

  // export it
  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew<v8::String>("Document"), tpl->GetFunction());

  // export standalone functions
  exports->Set(NanNew<v8::String>("parseFromString"), FUNCTION_VALUE(ParseFromString));
}

/**
 * Constructor
 */
Document::Document(xmlDocPtr doc) : Node((xmlNodePtr)doc) {
}

/**
 * Destructor
 */
Document::~Document() {
  if (doc())
    xmlFreeDoc(doc());
}

/**
 * `new Document`
 */
NAN_METHOD(Document::New) {
  NanScope();
  
  // must be invoked as `new Document()`
  if ( (! args.IsConstructCall()) || (! (args.Length() == 0)) )
    ThrowEx("Document constructor called incorrectly");
    
  Document* obj = new Document((xmlDocPtr)0);
  assertPointerValid(obj);
  
  obj->Wrap(args.This());
  
  NanReturnThis();
}

/**
 * Parse an XML document from a string
 */
NAN_METHOD(Document::ParseFromString) {
  NanScope();

  v8::String::Utf8Value xmlStr(args[0]->ToString());

  xmlDocPtr doc = xmlParseMemory((const char*) *xmlStr, xmlStr.length());
  if (!doc)
    ThrowEx("Invalid XML");
  
  v8::Local<v8::Object> retObj = NanNew(constructor)->NewInstance();

  Document* obj = node::ObjectWrap::Unwrap<Document>(retObj);
  if (!obj) {
    xmlFreeDoc(doc);
    NanReturnValue(retObj);
  }
  
  obj->doc(doc);
  
  NanReturnValue(retObj);
}


} // namespace xmlselector
