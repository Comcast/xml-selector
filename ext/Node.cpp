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
#include "Element.h"
#include "CharacterData.h"
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
  
  NanSetPrototypeTemplate(tpl, "hasChildNodes", FUNCTION_VALUE(HasChildNodes));

  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("nodeType"), NodeType);
  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("nodeName"), NodeName);
  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("firstChild"), FirstChild);
  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("lastChild"), LastChild);
  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("parentNode"), ParentNode);
  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("nextSibling"), NextSibling);
  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("previousSibling"), PreviousSibling);
  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("ownerDocument"), OwnerDocument);

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
  if (_node && _node->_private)
    _node->_private = 0;
}

/**
 * Return the corresponding Javascript object for an XML node
 */
v8::Local<v8::Object> Node::New(xmlNodePtr n) {
  NanEscapableScope();
  
  if (n->_private)
    return NanEscapeScope(NanObjectWrapHandle( ((Node*)n->_private) ));
  
  switch (n->type) {
  case XML_ELEMENT_NODE:
    return NanEscapeScope(wrapNode(n, Element::constructor));
  case XML_TEXT_NODE:
  case XML_CDATA_SECTION_NODE:
  case XML_COMMENT_NODE:
    return NanEscapeScope(wrapNode(n, CharacterData::constructor));
  default:
    return NanEscapeScope(wrapNode(n, Node::constructor));
  }
}

/**
 * Handles creating a new Javascript object to wrap an XML node
 */
v8::Local<v8::Object> Node::wrapNode(xmlNodePtr n, v8::Persistent<v8::Function>& ctor) {
  v8::Local<v8::Object> retObj = NanNew(ctor)->NewInstance();
  if (retObj.IsEmpty()) return retObj;

  Node* obj = node::ObjectWrap::Unwrap<Node>(retObj);
  if (!obj)
    return retObj;

  obj->node(n);
  n->_private = obj;
  
  // ensures the document remains in scope as long as some if its contents are referenced
  if ( n->doc && ( ((xmlNodePtr)n->doc) != n ) )
    retObj->SetHiddenValue(NanNew<v8::String>("_doc"), Node::New((xmlNodePtr)n->doc));

  return retObj;
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

/**
 * hasChildNodes() - returns boolean - DOM Level 1
 */
NAN_METHOD(Node::HasChildNodes) {
  NanScope();
  
  Node* obj = node::ObjectWrap::Unwrap<Node>(args.This());
  assertGotWrapper(obj);
  
  if (obj->node() && obj->node()->children)
    NanReturnValue(NanTrue());
  else
    NanReturnValue(NanFalse());
}

/**
 * nodeType - readonly attribute - DOM Level 1
 */
NAN_PROPERTY_GETTER(Node::NodeType) {
  NanScope();
  
  Node* obj = node::ObjectWrap::Unwrap<Node>(args.This());
  assertGotWrapper(obj);
  assertHasNode(obj);

  NanReturnValue(NanNew<v8::Integer>((int)obj->node()->type));
}

/**
 * nodeName - readonly attribute - DOM Level 1
 */
NAN_PROPERTY_GETTER(Node::NodeName) {
  NanScope();
  
  Node* obj = node::ObjectWrap::Unwrap<Node>(args.This());
  assertGotWrapper(obj);
  assertHasNode(obj);

  if (!obj->node()->name)
    NanReturnNull();
  else
    NanReturnValue(NewUtf8Handle((const char*)obj->node()->name));
}

/**
 * firstChild - readonly attribute - DOM Level 1
 */
NAN_PROPERTY_GETTER(Node::FirstChild) {
  NanScope();
  
  Node* obj = node::ObjectWrap::Unwrap<Node>(args.This());
  assertGotWrapper(obj);
  assertHasNode(obj);

  if (!obj->node()->children)
    NanReturnNull();
  else
    NanReturnValue(New(obj->node()->children));
}

/**
 * lastChild - readonly attribute - DOM Level 1
 */
NAN_PROPERTY_GETTER(Node::LastChild) {
  NanScope();
  
  Node* obj = node::ObjectWrap::Unwrap<Node>(args.This());
  assertGotWrapper(obj);
  assertHasNode(obj);

  if (!obj->node()->last)
    NanReturnNull();
  else
    NanReturnValue(New(obj->node()->last));
}

/**
 * parentNode - readonly attribute - DOM Level 1
 */
NAN_PROPERTY_GETTER(Node::ParentNode) {
  NanScope();
  
  Node* obj = node::ObjectWrap::Unwrap<Node>(args.This());
  assertGotWrapper(obj);
  assertHasNode(obj);

  if (!obj->node()->parent)
    NanReturnNull();
  else
    NanReturnValue(New(obj->node()->parent));
}

/**
 * nextSibling - readonly attribute - DOM Level 1
 */
NAN_PROPERTY_GETTER(Node::NextSibling) {
  NanScope();
  
  Node* obj = node::ObjectWrap::Unwrap<Node>(args.This());
  assertGotWrapper(obj);
  assertHasNode(obj);

  if (!obj->node()->next)
    NanReturnNull();
  else
    NanReturnValue(New(obj->node()->next));
}

/**
 * previousSibling - readonly attribute - DOM Level 1
 */
NAN_PROPERTY_GETTER(Node::PreviousSibling) {
  NanScope();
  
  Node* obj = node::ObjectWrap::Unwrap<Node>(args.This());
  assertGotWrapper(obj);
  assertHasNode(obj);

  if (!obj->node()->prev)
    NanReturnNull();
  else
    NanReturnValue(New(obj->node()->prev));
}

/**
 * ownerDocument - readonly attribute - DOM Level 1
 */
NAN_PROPERTY_GETTER(Node::OwnerDocument) {
  NanScope();
  
  Node* obj = node::ObjectWrap::Unwrap<Node>(args.This());
  assertGotWrapper(obj);
  assertHasNode(obj);

  if ( (!obj->node()->doc) ||
       (obj->node()->type == XML_DOCUMENT_NODE && obj->node()->doc == (xmlDocPtr)obj->node()) )
    NanReturnNull();
  else
    NanReturnValue(New((xmlNodePtr)obj->node()->doc));
}


} // namespace xmlselector
