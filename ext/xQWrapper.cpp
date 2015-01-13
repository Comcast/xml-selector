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
#include "xQWrapper.h"
#include <xml_node.h>
#include <xml_element.h>
#include <xml_document.h>

static const char* _xqErrors[] = {
  "OK",
  "Out of memory",
  "Argument out of bounds",
  "String could not be parsed as XML",
  "internal error code",
  "Unterminated string in selector",
  "Invalid selector",
  "internal error code",
  "Unknown namespace prefix",
  NULL
};

#define xQStatusString(code) ((code > 8) ? "Unknown error" : _xqErrors[code])

#define assertPointerValid(ptr) \
  if (!(ptr)) return v8::ThrowException(v8::Exception::Error(v8::String::New("Out of memory")));

#define assertGotWrapper(ptr) \
  if (!(ptr)) return v8::ThrowException(v8::Exception::Error(v8::String::New("xQ function invoked on an invalid instance")));

#define assertGotLibxmljs(ptr) assertGotWrapper(ptr)

#define statusToException(code) \
  v8::ThrowException(v8::Exception::Error(v8::String::New(xQStatusString(code))))

#define assertStatusOK(code) \
  if ((code) != XQ_OK) return statusToException(code);

v8::Persistent<v8::Function> xQWrapper::constructor;

/**
 * Initialize the class
 */
void xQWrapper::Init(v8::Handle<v8::Object> exports) {
  // create a constructor function
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New);
  
  tpl->SetClassName(v8::String::NewSymbol("xQ"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  // populate the prototype
  v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();
  proto->Set(v8::String::NewSymbol("addNamespace"), v8::FunctionTemplate::New(AddNamespace)->GetFunction());
  proto->Set(v8::String::NewSymbol("attr"), v8::FunctionTemplate::New(Attr)->GetFunction());
  proto->Set(v8::String::NewSymbol("children"), v8::FunctionTemplate::New(Children)->GetFunction());
  proto->Set(v8::String::NewSymbol("closest"), v8::FunctionTemplate::New(Closest)->GetFunction());
  proto->Set(v8::String::NewSymbol("forEach"), v8::FunctionTemplate::New(ForEach)->GetFunction());
  proto->Set(v8::String::NewSymbol("filter"), v8::FunctionTemplate::New(Filter)->GetFunction());
  proto->Set(v8::String::NewSymbol("search"), v8::FunctionTemplate::New(Find)->GetFunction());
  proto->Set(v8::String::NewSymbol("findIndex"), v8::FunctionTemplate::New(FindIndex)->GetFunction());
  proto->Set(v8::String::NewSymbol("first"), v8::FunctionTemplate::New(First)->GetFunction());
  proto->Set(v8::String::NewSymbol("last"), v8::FunctionTemplate::New(Last)->GetFunction());
  proto->SetAccessor(v8::String::NewSymbol("length"), GetLength);
  proto->Set(v8::String::NewSymbol("next"), v8::FunctionTemplate::New(Next)->GetFunction());
  proto->Set(v8::String::NewSymbol("nextAll"), v8::FunctionTemplate::New(NextAll)->GetFunction());
  proto->Set(v8::String::NewSymbol("nextUntil"), v8::FunctionTemplate::New(NextUntil)->GetFunction());
  proto->Set(v8::String::NewSymbol("not"), v8::FunctionTemplate::New(Not)->GetFunction());
  proto->Set(v8::String::NewSymbol("parent"), v8::FunctionTemplate::New(Parent)->GetFunction());
  proto->Set(v8::String::NewSymbol("parents"), v8::FunctionTemplate::New(Parents)->GetFunction());
  proto->Set(v8::String::NewSymbol("parentsUntil"), v8::FunctionTemplate::New(ParentsUntil)->GetFunction());
  proto->Set(v8::String::NewSymbol("prev"), v8::FunctionTemplate::New(Prev)->GetFunction());
  proto->Set(v8::String::NewSymbol("prevAll"), v8::FunctionTemplate::New(PrevAll)->GetFunction());
  proto->Set(v8::String::NewSymbol("prevUntil"), v8::FunctionTemplate::New(PrevUntil)->GetFunction());
  proto->Set(v8::String::NewSymbol("text"), v8::FunctionTemplate::New(Text)->GetFunction());
  proto->Set(v8::String::NewSymbol("xml"), v8::FunctionTemplate::New(Xml)->GetFunction());
  
  proto->SetIndexedPropertyHandler(GetIndex, SetIndex, QueryIndex, DeleteIndex, EnumIndicies);

  
  // export it
  constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
  exports->Set(v8::String::NewSymbol("xQ"), constructor);
}

/**
 * Create a new wrapped xQWrapper. This is intended for use by C++ callers.
 */
v8::Local<v8::Object> xQWrapper::New(xQ* xq) {
  v8::Local<v8::Object> retObj = constructor->NewInstance();

  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(retObj);
  if (!obj) {
    xQ_free(xq, 1);
    return retObj;
  }
  
  if (obj->_xq)
    xQ_free(obj->_xq, 1);
  
  obj->_xq = xq;
  obj->shadowNodeList(retObj);

  return retObj;
}

/**
 * Destructor
 */
xQWrapper::~xQWrapper() {
  if (_xq)
    xQ_free(_xq, 1);
  _xq = 0;
}

/**
 * Create a shadow node list that contains the nodes in our list as
 * objects accessible to JavaScript. This is done at object creation time
 * to simplify memory management.
 */
void xQWrapper::shadowNodeList(v8::Local<v8::Object> wrapper) {
  int len = (int) xQ_length(_xq);
  v8::Local<v8::Array> list = v8::Array::New(len);
  
  for (int i = 0; i < len; i++) {
    xmlNodePtr node = _xq->context.list[i];

    if (node->type == XML_DOCUMENT_NODE)
      list->Set(i, libxmljs::XmlDocument::New((xmlDocPtr)node));
    else
      list->Set(i, libxmljs::XmlNode::New(node));
  }
  
  wrapper->SetHiddenValue(v8::String::NewSymbol("_nodes"), list);
}

/**
 * Utility routine to add a JS object to a node list
 */
static v8::Handle<v8::Value> addToNodeList(xQ* q, v8::Local<v8::Value> val) {
  v8::Local<v8::TypeSwitch> elemType = v8::TypeSwitch::New(libxmljs::XmlElement::constructor_template);
  v8::Local<v8::String> docName = v8::String::New("Document");
  
  if (val->IsObject()) {
    
    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(val);
    
    if (obj->GetConstructorName()->StrictEquals(docName)) {
      
      libxmljs::XmlDocument* doc = node::ObjectWrap::Unwrap<libxmljs::XmlDocument>(obj);
      assertGotLibxmljs(doc);
      
      xQStatusCode result = xQNodeList_push(&(q->context), (xmlNodePtr) doc->xml_obj);
      assertStatusOK(result);
      
    } else if (elemType->match(obj)) {
      
      libxmljs::XmlNode* elem = node::ObjectWrap::Unwrap<libxmljs::XmlNode>(obj);
      assertGotLibxmljs(elem);
      
      xQStatusCode result = xQNodeList_push(&(q->context), (xmlNodePtr) elem->xml_obj);
      assertStatusOK(result);
      
    } else {
      return v8::ThrowException(v8::Exception::Error(v8::String::New("Unsupported item in xQ constructor")));
    }

  } else {
    return v8::ThrowException(v8::Exception::Error(v8::String::New("Unsupported item in xQ constructor")));
  }
  
  return v8::True();
}
 
/**
 * `new xQ(...)`
 */
v8::Handle<v8::Value> xQWrapper::New(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  // must be invoked as `new xQ([/* list of nodes */])`
  if ( (! args.IsConstructCall()) ||
       (! (args.Length() == 0 || (args.Length() == 1 && args[0]->IsArray())) ) )
    return v8::ThrowException(v8::Exception::Error(v8::String::New("xQ constructor called incorrectly")));
    

  xQWrapper* obj = new xQWrapper();
  assertPointerValid(obj);
  
  xQStatusCode result = XQ_OK;

  result = xQ_alloc_init(&(obj->_xq));

  if (result == XQ_OK && args.Length() > 0) {

    v8::TryCatch tryBlock;
    v8::Local<v8::Array> ary = v8::Local<v8::Array>::Cast(args[0]);
    uint32_t len = ary->Length();

    for (uint32_t i = 0; i < len; i++) {
      addToNodeList(obj->_xq, ary->Get(i));
    
      if (tryBlock.HasCaught()) {
        delete obj;
        return tryBlock.ReThrow();
      }
    }
  }

  if (result != XQ_OK) {
    delete obj;
    return statusToException(result);
  }
  
  obj->Wrap(args.This());
  obj->shadowNodeList(args.This());
  
  return args.This();
}

/**
 * Associate a namespace prefix for selectors with a URI
 */
v8::Handle<v8::Value> xQWrapper::AddNamespace(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value prefix(args[0]->ToString());
  v8::String::Utf8Value uri(args[1]->ToString());

  xQStatusCode result = xQ_addNamespace(obj->_xq, (xmlChar*) *prefix, (xmlChar*) *uri);
  assertStatusOK(result);

  return args.This();
}

/**
 * Return the value of the named attribute from the first node in the list
 */
v8::Handle<v8::Value> xQWrapper::Attr(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value name(args[0]->ToString());

  xmlChar* txt = xQ_getAttr(obj->_xq, *name);
  
  if (!txt)
    return v8::Undefined();
  
  v8::Local<v8::String> retTxt = v8::String::New((const char*)txt);
  
  xmlFree(txt);
  
  return scope.Close(retTxt);
}

/**
 * Return a new xQ instance containing the children of the nodes from this
 * set, optionally filtered by a selector
 */
v8::Handle<v8::Value> xQWrapper::Children(const v8::Arguments& args) {
  v8::HandleScope scope;
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_children(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the nearest ancestors of this set
 * that match the provided selector
 */
v8::Handle<v8::Value> xQWrapper::Closest(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_closest(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Iterate over the items in this collection, passing each to a user-supplied callback
 */
v8::Handle<v8::Value> xQWrapper::ForEach(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  if (args.Length() < 1 || !args[0]->IsFunction())
    return args.This();

  v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);

  v8::Handle<v8::Object> thisArg;
  if (args.Length() > 1 && args[1]->IsObject()) {
    thisArg = v8::Local<v8::Object>::Cast(args[1]);
  } else {
    thisArg = v8::Context::GetCurrent()->Global();
  }
  
  uint32_t len = (uint32_t) xQ_length(obj->_xq);
  v8::Local<v8::Array> list = v8::Local<v8::Array>::Cast(args.This()->GetHiddenValue(v8::String::NewSymbol("_nodes")));
  
  v8::TryCatch tryBlock;
  
  for (uint32_t i = 0; i < len; i++) {
    const unsigned argc = 3;
    v8::Local<v8::Value> argv[] = {list->Get(i), v8::Integer::NewFromUnsigned(i), args.This()};

    callback->Call(thisArg, argc, argv);
    
    if (tryBlock.HasCaught())
      return tryBlock.ReThrow();
  }
  
  return args.This();
}

/**
 * Return a new xQ instance containing the nodes from this set that match
 * the provided selector
 */
v8::Handle<v8::Value> xQWrapper::Filter(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_filter(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Search the nodes in this set for descendants matching the provided
 * selector and return a new xQ with the results
 */
v8::Handle<v8::Value> xQWrapper::Find(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_find(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Iterate over the items in this collection, passing each to a
 * user-supplied callback. Returns the index of the first item in the
 * collection for which the user-supplied callback returns true. Returns
 * -1 if the callback does not return true for any item.
 */
v8::Handle<v8::Value> xQWrapper::FindIndex(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  if (args.Length() < 1 || !args[0]->IsFunction())
    return scope.Close(v8::Integer::New(-1));

  v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);

  v8::Handle<v8::Object> thisArg;
  if (args.Length() > 1 && args[1]->IsObject()) {
    thisArg = v8::Local<v8::Object>::Cast(args[1]);
  } else {
    thisArg = v8::Context::GetCurrent()->Global();
  }
  
  uint32_t len = (uint32_t) xQ_length(obj->_xq);
  v8::Local<v8::Array> list = v8::Local<v8::Array>::Cast(args.This()->GetHiddenValue(v8::String::NewSymbol("_nodes")));
  
  v8::TryCatch tryBlock;
  
  for (uint32_t i = 0; i < len; i++) {
    const unsigned argc = 3;
    v8::Local<v8::Value> argv[] = {list->Get(i), v8::Integer::NewFromUnsigned(i), args.This()};

    v8::Local<v8::Value> result = callback->Call(thisArg, argc, argv);
    
    if (tryBlock.HasCaught())
      return tryBlock.ReThrow();
    
    if (result->BooleanValue())
      return v8::Integer::NewFromUnsigned(i);
  }
  
  return v8::Integer::New(-1);
}

/**
 * Return a new xQ instance containing the the first node from this xQ's set
 */
v8::Handle<v8::Value> xQWrapper::First(const v8::Arguments& args) {
  v8::HandleScope scope;
  xQ* out = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  xQStatusCode result = xQ_first(obj->_xq, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the the last node from this xQ's set
 */
v8::Handle<v8::Value> xQWrapper::Last(const v8::Arguments& args) {
  v8::HandleScope scope;
  xQ* out = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  xQStatusCode result = xQ_last(obj->_xq, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return the length/size/count of the xQ instance
 */
v8::Handle<v8::Value> xQWrapper::GetLength(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(info.This());
  assertGotWrapper(obj);
  
  return scope.Close(v8::Number::New((double)xQ_length(obj->_xq)));
}

/**
 * Return a new xQ instance containing the next immediate sibling of each
 * node in this set, optionally filtered by a selector
 */
v8::Handle<v8::Value> xQWrapper::Next(const v8::Arguments& args) {
  v8::HandleScope scope;
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_next(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the all the next siblings of each
 * node in this set, optionally filtered by a selector
 */
v8::Handle<v8::Value> xQWrapper::NextAll(const v8::Arguments& args) {
  v8::HandleScope scope;
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_nextAll(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing all the next siblings of the nodes
 * in this set up to ones matching the supplied selector
 */
v8::Handle<v8::Value> xQWrapper::NextUntil(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_nextUntil(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing all the nodes in this set which
 * do not match the supplied selector
 */
v8::Handle<v8::Value> xQWrapper::Not(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_not(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the parent node of each
 * node in this set, optionally filtered by a selector
 */
v8::Handle<v8::Value> xQWrapper::Parent(const v8::Arguments& args) {
  v8::HandleScope scope;
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_parent(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the all the ancestors of each
 * node in this set, optionally filtered by a selector
 */
v8::Handle<v8::Value> xQWrapper::Parents(const v8::Arguments& args) {
  v8::HandleScope scope;
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_parents(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing all the ancestors of the nodes
 * in this set up to ones matching the supplied selector
 */
v8::Handle<v8::Value> xQWrapper::ParentsUntil(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_parentsUntil(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the prevous immediate sibling of each
 * node in this set, optionally filtered by a selector
 */
v8::Handle<v8::Value> xQWrapper::Prev(const v8::Arguments& args) {
  v8::HandleScope scope;
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_prev(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the all the previous siblings of each
 * node in this set, optionally filtered by a selector
 */
v8::Handle<v8::Value> xQWrapper::PrevAll(const v8::Arguments& args) {
  v8::HandleScope scope;
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_prevAll(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing all the previous siblings of the nodes
 * in this set up to ones matching the supplied selector
 */
v8::Handle<v8::Value> xQWrapper::PrevUntil(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_prevUntil(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  return scope.Close(xQWrapper::New(out));
}

/**
 * Return the text content of the first node in the list
 */
v8::Handle<v8::Value> xQWrapper::Text(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  xmlChar* txt = xQ_getText(obj->_xq);
  assertPointerValid(txt);
  
  v8::Local<v8::String> retTxt = v8::String::New((const char*)txt);
  
  xmlFree(txt);
  
  return scope.Close(retTxt);
}

/**
 * Return the xml content of the first node in the list
 */
v8::Handle<v8::Value> xQWrapper::Xml(const v8::Arguments& args) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  xmlChar* txt = xQ_getXml(obj->_xq);
  assertPointerValid(txt);
  
  v8::Local<v8::String> retTxt = v8::String::New((const char*)txt);
  
  xmlFree(txt);
  
  return scope.Close(retTxt);
}



/**
 * Return the value at the given index
 */
v8::Handle<v8::Value> xQWrapper::GetIndex(uint32_t index, const v8::AccessorInfo& info) {
  v8::HandleScope scope;
  
  v8::Local<v8::Array> list = v8::Local<v8::Array>::Cast(info.This()->GetHiddenValue(v8::String::NewSymbol("_nodes")));
  
  return scope.Close(list->Get(index));
}

/**
 * Return the property attributes of the indexed item
 */
v8::Handle<v8::Integer> xQWrapper::QueryIndex(uint32_t index, const v8::AccessorInfo& info) {
  v8::HandleScope scope;

  v8::Local<v8::Integer> props;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(info.This());
  if (!obj)
    return scope.Close(props);
  
  uint32_t len = (uint32_t) xQ_length(obj->_xq);
  
  if (index < len)
    props = v8::Integer::New((int)v8::ReadOnly);

  return scope.Close(props);
}

/**
 * Disallow setting indexed values
 */
v8::Handle<v8::Value> xQWrapper::SetIndex(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
  return value;
}

/**
 * Disallow deleting values
 */
v8::Handle<v8::Boolean> xQWrapper::DeleteIndex(uint32_t index, const v8::AccessorInfo& info) {
  return v8::False();
}

v8::Handle<v8::Array> xQWrapper::EnumIndicies(const v8::AccessorInfo& info) {
  v8::HandleScope scope;
  v8::Local<v8::Array> idxs = v8::Array::New();
  
  return scope.Close(idxs);
}
