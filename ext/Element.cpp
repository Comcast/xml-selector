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
#include "Element.h"
#include "utils.h"

namespace xmlselector {


v8::Persistent<v8::Function> Element::constructor;

/**
 * Class initialization and exports
 */
void Element::Init(v8::Handle<v8::Object> exports) {
  // create a constructor function
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);

  tpl->SetClassName(NanNew<v8::String>("Element"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  // inherits from Node
  tpl->Inherit(NanNew(Node::constructor_template));

  NanSetPrototypeTemplate(tpl, "getAttribute", FUNCTION_VALUE(GetAttribute));

  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("tagName"), TagName);

  // export it
  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew<v8::String>("Element"), tpl->GetFunction());
}

/**
 * Constructor
 */
Element::Element(xmlElementPtr elem) : Node((xmlNodePtr)elem) {
}

/**
 * Destructor
 */
Element::~Element() {
}

/**
 * `new Element`
 */
NAN_METHOD(Element::New) {
  NanScope();
  
  // must be invoked as `new Element()`
  if ( (! args.IsConstructCall()) || (! (args.Length() == 0)) )
    ThrowEx("Element constructor called incorrectly");
    
  Element* obj = new Element((xmlElementPtr)0);
  assertPointerValid(obj);
  
  obj->Wrap(args.This());
  
  NanReturnThis();
}

/**
 * getAttribute(name) - returns String - DOM Level 1
 */
NAN_METHOD(Element::GetAttribute) {
  NanScope();
  
  Element* obj = node::ObjectWrap::Unwrap<Element>(args.This());
  assertGotWrapper(obj);
  
  if (!obj->node())
    NanReturnEmptyString();
  
  v8::String::Utf8Value name(args[0]->ToString());
    
  xmlChar* value = xmlGetProp(obj->node(), (const xmlChar*)*name);
  
  if (!value)
    NanReturnEmptyString();
  
  v8::Local<v8::String> str = NewUtf8Handle((char*)value);
  
  xmlFree(value);
    
  NanReturnValue(str);
}

/**
 * tagName - readonly attribute - DOM Level 1
 */
NAN_PROPERTY_GETTER(Element::TagName) {
  NanScope();
  
  Node* obj = node::ObjectWrap::Unwrap<Node>(args.This());
  assertGotWrapper(obj);
  assertHasNode(obj);

  if (!obj->node()->name)
    NanReturnNull();
  else
    NanReturnValue(NewUtf8Handle((const char*)obj->node()->name));
}


} // namespace xmlselector
