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
#include "Node.h"
#include "utils.h"

namespace xmlselector {


v8::Persistent<v8::FunctionTemplate> Node::constructor_template;
v8::Persistent<v8::Function> Node::constructor;

/**
 * Class initialization and exports
 */
void Node::Init(v8::Handle<v8::Object> exports) {
  // create a constructor function
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);

  tpl->SetClassName(NanNew<v8::String>("Node"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // export it
  NanAssignPersistent(constructor_template, tpl);
  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew<v8::String>("Node"), tpl->GetFunction());
}

/**
 * Constructor
 */
Node::Node(xmlNodePtr n) : _node(n) {
}

/**
 * Destructor
 */
Node::~Node() {
}

/**
 * `new Node`
 */
NAN_METHOD(Node::New) {
  NanScope();
  
  // must be invoked as `new Node()`
  if ( (! args.IsConstructCall()) || (! (args.Length() == 0)) )
    ThrowEx("Node constructor called incorrectly");
    
  Node* obj = new Node((xmlNodePtr)0);
  assertPointerValid(obj);
  
  obj->Wrap(args.This());
  
  NanReturnThis();
}


} // namespace xmlselector
