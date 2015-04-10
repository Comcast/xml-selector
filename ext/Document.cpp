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

#include <stdarg.h>

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

  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("documentElement"), DocumentElement);

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
  if (doc()) {
    xmlDocPtr d = doc();
    cleanTree((xmlNodePtr)d);
    xmlFreeDoc(d);
  }
}

/**
 * Cleans the tree of javascript references before deletion
 */
void Document::cleanTree(xmlNodePtr n) {
  if (n->_private) {
    ((Node*)n->_private)->node(0);
    n->_private = 0;
  }
  
  if (n->next) cleanTree(n->next);
  if (n->children) cleanTree(n->children);
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
 * Error handling routing for parsing
 */
void parseErrorHandler(void *ctx, const char *msg, ...) {
    va_list args;
    v8::Array* errors = (v8::Array*)ctx;

    // determine the size of the message
    va_start(args, msg);
    
    int strSize = vsnprintf(0, 0, msg, args);
    if (strSize < 1) strSize = 1024;
    
    va_end(args);
    
    // allocate memory for the formatted message
    char* str = new char[strSize + 1];
    if (!str) return;
    str[0] = 0;

    // format it
    va_start(args, msg);
    
    vsnprintf(str, strSize + 1, msg, args);
    
    va_end(args);
    
    str[strSize] = 0;
    
    errors->Set(errors->Length(), NanNew<v8::String>(str));
    
    delete[] str;
}

/**
 * Parse an XML document from a string
 */
NAN_METHOD(Document::ParseFromString) {
  NanScope();

  v8::String::Utf8Value xmlStr(args[0]->ToString());

  v8::Local<v8::Array> errors = NanNew<v8::Array>();
  xmlSetGenericErrorFunc(*errors, parseErrorHandler);
  
  xmlDocPtr doc = xmlParseMemory((const char*) *xmlStr, xmlStr.length());
  
  xmlSetGenericErrorFunc(0, 0);
  
  if (!doc) {
    v8::Local<v8::String> errStr;
    
    if (errors->Length() < 1)
      errStr = NanNew<v8::String>("Invalid XML");
    else
      errStr = errors->Get(0)->ToString();
    
    for (uint32_t i = 1; i < errors->Length(); i++)
      errStr = v8::String::Concat(errStr, errors->Get(i)->ToString());
    
    ThrowEx(v8::Exception::Error(errStr));
  }
  
  v8::Local<v8::Object> retObj = NanNew(constructor)->NewInstance();

  Document* obj = node::ObjectWrap::Unwrap<Document>(retObj);
  if (!obj) {
    xmlFreeDoc(doc);
    NanReturnValue(retObj);
  }
  
  obj->doc(doc);
  doc->_private = obj;
  
  NanReturnValue(retObj);
}

/**
 * documentElement - readonly attribute - DOM Level 1
 */
NAN_PROPERTY_GETTER(Document::DocumentElement) {
  NanScope();
  
  Document* obj = node::ObjectWrap::Unwrap<Document>(args.This());
  assertGotWrapper(obj);
  assertHasNode(obj);

  xmlNodePtr n = obj->doc()->children;
  
  while (n) {
    if (n->type == XML_ELEMENT_NODE)
      NanReturnValue(Node::New(n));

    n = n->next;
  }

  NanReturnNull();
}


} // namespace xmlselector
