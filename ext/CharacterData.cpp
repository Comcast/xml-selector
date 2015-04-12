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
#include "CharacterData.h"
#include "utils.h"

namespace xmlselector {


v8::Persistent<v8::Function> CharacterData::constructor;

/**
 * Class initialization and exports
 */
void CharacterData::Init(v8::Handle<v8::Object> exports) {
  // create a constructor function
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);

  tpl->SetClassName(NanNew<v8::String>("CharacterData"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  // inherits from Node
  tpl->Inherit(NanNew(Node::constructor_template));

  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("data"), Data);
  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("length"), Length);

  // export it
  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew<v8::String>("CharacterData"), tpl->GetFunction());
}

/**
 * Constructor
 */
CharacterData::CharacterData(xmlNodePtr n) : Node(n) {
}

/**
 * Destructor
 */
CharacterData::~CharacterData() {
}

/**
 * `new CharacterData`
 */
NAN_METHOD(CharacterData::New) {
  NanScope();
  
  // must be invoked as `new CharacterData()`
  if ( (! args.IsConstructCall()) || (! (args.Length() == 0)) )
    ThrowEx("CharacterData constructor called incorrectly");
    
  CharacterData* obj = new CharacterData((xmlNodePtr)0);
  assertPointerValid(obj);
  
  obj->Wrap(args.This());
  
  NanReturnThis();
}

/**
 * data - read/write attribute - DOM Level 1
 */
NAN_PROPERTY_GETTER(CharacterData::Data) {
  NanScope();
  
  Node* obj = node::ObjectWrap::Unwrap<Node>(args.This());
  assertGotWrapper(obj);
  assertHasNode(obj);

  xmlChar* content = xmlNodeGetContent(obj->node());
  
  if (!content)
    NanReturnNull();
  
  v8::Local<v8::String> data = NewUtf8Handle((char*)content);
  
  xmlFree(content);
  
  NanReturnValue(data);
}

/**
 * length - readonly attribute - DOM Level 1
 */
NAN_PROPERTY_GETTER(CharacterData::Length) {
  NanScope();
  
  Node* obj = node::ObjectWrap::Unwrap<Node>(args.This());
  assertGotWrapper(obj);

  if (!obj->node())
    NanReturnValue(NanNew<v8::Integer>(0));
  
  if ( (obj->node()->type == XML_TEXT_NODE) ||
       (obj->node()->type == XML_CDATA_SECTION_NODE) ||
       (obj->node()->type == XML_COMMENT_NODE) )
         NanReturnValue(NanNew<v8::Integer>(xmlUTF8Strlen(obj->node()->content)));
  
  NanReturnValue(NanNew<v8::Integer>(0));
}


} // namespace xmlselector
