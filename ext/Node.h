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
#ifndef __XMLSELECTOR_NODE_H_INCLUDED__
#define __XMLSELECTOR_NODE_H_INCLUDED__

#include <node.h>
#include <nan.h>

#include <libxml/tree.h>

namespace xmlselector {

class Node : public node::ObjectWrap {
public:
  static void Init(v8::Handle<v8::Object> exports);

  static v8::Persistent<v8::FunctionTemplate> constructor_template;
  static v8::Persistent<v8::Function> constructor;
  
  static v8::Local<v8::Object> New(xmlNodePtr n);

  xmlNodePtr node() { return _node; }
  void node(xmlNodePtr newNode) { _node = newNode; }

protected:

  explicit Node(xmlNodePtr doc);
  virtual ~Node();
  
  static v8::Local<v8::Object> wrapNode(xmlNodePtr n, v8::Persistent<v8::Function>& ctor);

  static NAN_METHOD(New);
  static NAN_METHOD(HasChildNodes);
  
  static NAN_PROPERTY_GETTER(NodeType);
  static NAN_PROPERTY_GETTER(NodeName);
  static NAN_PROPERTY_GETTER(FirstChild);
  static NAN_PROPERTY_GETTER(LastChild);
  static NAN_PROPERTY_GETTER(ParentNode);
  static NAN_PROPERTY_GETTER(NextSibling);
  static NAN_PROPERTY_GETTER(PreviousSibling);
  static NAN_PROPERTY_GETTER(OwnerDocument);
  
  xmlNodePtr _node;
};

} // namespace xmlselector

#endif // __XMLSELECTOR_NODE_H_INCLUDED__