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

#if (NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION)
#define ThrowEx(msg) do { NanThrowError(msg); return; } while (0);
#define ReThrowEx(blk) do { blk.ReThrow(); return; } while (0);
#else
#define ThrowEx(msg) NanThrowError(msg)
#define ReThrow(blk) return blk.ReThrow();
#endif

#define xQStatusString(code) ((code > 8) ? "Unknown error" : _xqErrors[code])

#define assertPointerValid(ptr) \
  if (!(ptr)) ThrowEx("Out of memory");

#define assertGotWrapper(ptr) \
  if (!(ptr)) ThrowEx("xQ function invoked on an invalid instance");

#define assertGotLibxmljs(ptr) assertGotWrapper(ptr)

#define statusToException(code) \
  ThrowEx(xQStatusString(code))

#define assertStatusOK(code) \
  if ((code) != XQ_OK) statusToException(code);

#define FUNCTION_VALUE(f) \
  NanNew<v8::FunctionTemplate>(f)->GetFunction()

// TODO: upgrade NAN
NAN_INLINE void NanSetPrototypeTemplate(
    v8::Local<v8::FunctionTemplate> templ
  , const char *name
  , v8::Handle<v8::Data> value
) {
  NanSetTemplate(templ->PrototypeTemplate(), name, value);
}
#define NanReturnThis() NanReturnValue(args.This())
// end TODO

v8::Persistent<v8::Function> xQWrapper::constructor;

/**
 * Initialize the class
 */
void xQWrapper::Init(v8::Handle<v8::Object> exports) {
  // create a constructor function
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);
  
  tpl->SetClassName(NanNew<v8::String>("xQ"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  // populate the prototype
  NanSetPrototypeTemplate(tpl, "addNamespace", FUNCTION_VALUE(AddNamespace));
  NanSetPrototypeTemplate(tpl, "attr", FUNCTION_VALUE(Attr));
  NanSetPrototypeTemplate(tpl, "children", FUNCTION_VALUE(Children));
  NanSetPrototypeTemplate(tpl, "closest", FUNCTION_VALUE(Closest));
  NanSetPrototypeTemplate(tpl, "forEach", FUNCTION_VALUE(ForEach));
  NanSetPrototypeTemplate(tpl, "filter", FUNCTION_VALUE(Filter));
  NanSetPrototypeTemplate(tpl, "search", FUNCTION_VALUE(Find));
  NanSetPrototypeTemplate(tpl, "findIndex", FUNCTION_VALUE(FindIndex));
  NanSetPrototypeTemplate(tpl, "first", FUNCTION_VALUE(First));
  NanSetPrototypeTemplate(tpl, "last", FUNCTION_VALUE(Last));
  tpl->PrototypeTemplate()->SetAccessor(NanNew<v8::String>("length"), GetLength);
  NanSetPrototypeTemplate(tpl, "next", FUNCTION_VALUE(Next));
  NanSetPrototypeTemplate(tpl, "nextAll", FUNCTION_VALUE(NextAll));
  NanSetPrototypeTemplate(tpl, "nextUntil", FUNCTION_VALUE(NextUntil));
  NanSetPrototypeTemplate(tpl, "not", FUNCTION_VALUE(Not));
  NanSetPrototypeTemplate(tpl, "parent", FUNCTION_VALUE(Parent));
  NanSetPrototypeTemplate(tpl, "parents", FUNCTION_VALUE(Parents));
  NanSetPrototypeTemplate(tpl, "parentsUntil", FUNCTION_VALUE(ParentsUntil));
  NanSetPrototypeTemplate(tpl, "prev", FUNCTION_VALUE(Prev));
  NanSetPrototypeTemplate(tpl, "prevAll", FUNCTION_VALUE(PrevAll));
  NanSetPrototypeTemplate(tpl, "prevUntil", FUNCTION_VALUE(PrevUntil));
  NanSetPrototypeTemplate(tpl, "text", FUNCTION_VALUE(Text));
  NanSetPrototypeTemplate(tpl, "xml", FUNCTION_VALUE(Xml));
  
  tpl->PrototypeTemplate()->SetIndexedPropertyHandler(GetIndex, SetIndex, QueryIndex, DeleteIndex, EnumIndicies);

  
  // export it
  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew<v8::String>("xQ"), tpl->GetFunction());
}

/**
 * Create a new wrapped xQWrapper. This is intended for use by C++ callers.
 */
v8::Local<v8::Object> xQWrapper::New(xQ* xq) {
  NanEscapableScope();
  
  v8::Local<v8::Object> retObj = NanNew(constructor)->NewInstance();

  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(retObj);
  if (!obj) {
    xQ_free(xq, 1);
    NanEscapeScope(retObj);
    return retObj;
  }
  
  if (obj->_xq)
    xQ_free(obj->_xq, 1);
  
  obj->_xq = xq;
  obj->shadowNodeList(retObj);

  NanEscapeScope(retObj);

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
  v8::Local<v8::Array> list = NanNew<v8::Array>(len);
  
  for (int i = 0; i < len; i++) {
    xmlNodePtr node = _xq->context.list[i];

    if (node->type == XML_DOCUMENT_NODE)
      list->Set(i, libxmljs::XmlDocument::New((xmlDocPtr)node));
    else
      list->Set(i, libxmljs::XmlNode::New(node));
  }
  
  wrapper->SetHiddenValue(NanNew<v8::String>("_nodes"), list);
}

/**
 * Utility routine to add a JS object to a node list
 */
static _NAN_METHOD_RETURN_TYPE addToNodeList(xQ* q, v8::Local<v8::Value> val) {
  //v8::Local<v8::TypeSwitch> elemType = NanNew<v8::TypeSwitch>(libxmljs::XmlElement::constructor_template);
  v8::Local<v8::String> elemName = NanNew<v8::String>("Element");
  v8::Local<v8::String> docName = NanNew<v8::String>("Document");
  
  if (val->IsObject()) {
    
    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(val);
    
    if (obj->GetConstructorName()->StrictEquals(docName)) {
      
      libxmljs::XmlDocument* doc = node::ObjectWrap::Unwrap<libxmljs::XmlDocument>(obj);
      assertGotLibxmljs(doc);
      
      xQStatusCode result = xQNodeList_push(&(q->context), (xmlNodePtr) doc->xml_obj);
      assertStatusOK(result);
      
    } else if (obj->GetConstructorName()->StrictEquals(elemName)) {
    //} else if (elemType->match(obj)) {
      
      libxmljs::XmlNode* elem = node::ObjectWrap::Unwrap<libxmljs::XmlNode>(obj);
      assertGotLibxmljs(elem);
      
      xQStatusCode result = xQNodeList_push(&(q->context), (xmlNodePtr) elem->xml_obj);
      assertStatusOK(result);
      
    } else {
      ThrowEx("Unsupported item in xQ constructor");
    }

  } else {
    ThrowEx("Unsupported item in xQ constructor");
  }
  
  NanReturnUndefined();
}
 
/**
 * `new xQ(...)`
 */
NAN_METHOD(xQWrapper::New) {
  NanScope();
  
  // must be invoked as `new xQ([/* list of nodes */])`
  if ( (! args.IsConstructCall()) ||
       (! (args.Length() == 0 || (args.Length() == 1 && args[0]->IsArray())) ) )
    ThrowEx("xQ constructor called incorrectly");
    

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
        ReThrowEx(tryBlock);
      }
    }
  }

  if (result != XQ_OK) {
    delete obj;
    statusToException(result);
  }
  
  obj->Wrap(args.This());
  obj->shadowNodeList(args.This());
  
  NanReturnThis();
}

/**
 * Associate a namespace prefix for selectors with a URI
 */
NAN_METHOD(xQWrapper::AddNamespace) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value prefix(args[0]->ToString());
  v8::String::Utf8Value uri(args[1]->ToString());

  xQStatusCode result = xQ_addNamespace(obj->_xq, (xmlChar*) *prefix, (xmlChar*) *uri);
  assertStatusOK(result);

  NanReturnThis();
}

/**
 * Return the value of the named attribute from the first node in the list
 */
NAN_METHOD(xQWrapper::Attr) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value name(args[0]->ToString());

  xmlChar* txt = xQ_getAttr(obj->_xq, *name);
  
  if (!txt)
    NanReturnUndefined();
  
  v8::Local<v8::String> retTxt = NanNew<v8::String>((const char*)txt);
  
  xmlFree(txt);
  
  NanReturnValue(retTxt);
}

/**
 * Return a new xQ instance containing the children of the nodes from this
 * set, optionally filtered by a selector
 */
NAN_METHOD(xQWrapper::Children) {
  NanScope();
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_children(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the nearest ancestors of this set
 * that match the provided selector
 */
NAN_METHOD(xQWrapper::Closest) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_closest(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Iterate over the items in this collection, passing each to a user-supplied callback
 */
NAN_METHOD(xQWrapper::ForEach) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  if (args.Length() < 1 || !args[0]->IsFunction())
    NanReturnThis();

  v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);

  v8::Handle<v8::Object> thisArg;
  if (args.Length() > 1 && args[1]->IsObject()) {
    thisArg = v8::Local<v8::Object>::Cast(args[1]);
  } else {
    thisArg = NanGetCurrentContext()->Global();
  }
  
  uint32_t len = (uint32_t) xQ_length(obj->_xq);
  v8::Local<v8::Array> list = v8::Local<v8::Array>::Cast(args.This()->GetHiddenValue(NanNew<v8::String>("_nodes")));
  
  v8::TryCatch tryBlock;
  
  for (uint32_t i = 0; i < len; i++) {
    const unsigned argc = 3;
    v8::Local<v8::Value> argv[] = {list->Get(i), NanNew<v8::Integer>(i), args.This()};

    callback->Call(thisArg, argc, argv);
    
    if (tryBlock.HasCaught())
      ReThrowEx(tryBlock);
  }
  
  NanReturnThis();
}

/**
 * Return a new xQ instance containing the nodes from this set that match
 * the provided selector
 */
NAN_METHOD(xQWrapper::Filter) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_filter(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Search the nodes in this set for descendants matching the provided
 * selector and return a new xQ with the results
 */
NAN_METHOD(xQWrapper::Find) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_find(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Iterate over the items in this collection, passing each to a
 * user-supplied callback. Returns the index of the first item in the
 * collection for which the user-supplied callback returns true. Returns
 * -1 if the callback does not return true for any item.
 */
NAN_METHOD(xQWrapper::FindIndex) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  if (args.Length() < 1 || !args[0]->IsFunction())
    NanReturnValue(NanNew<v8::Integer>(-1));

  v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);

  v8::Handle<v8::Object> thisArg;
  if (args.Length() > 1 && args[1]->IsObject()) {
    thisArg = v8::Local<v8::Object>::Cast(args[1]);
  } else {
    thisArg = NanGetCurrentContext()->Global();
  }
  
  uint32_t len = (uint32_t) xQ_length(obj->_xq);
  v8::Local<v8::Array> list = v8::Local<v8::Array>::Cast(args.This()->GetHiddenValue(NanNew<v8::String>("_nodes")));
  
  v8::TryCatch tryBlock;
  
  for (uint32_t i = 0; i < len; i++) {
    const unsigned argc = 3;
    v8::Local<v8::Value> argv[] = {list->Get(i), NanNew<v8::Integer>(i), args.This()};

    v8::Local<v8::Value> result = callback->Call(thisArg, argc, argv);
    
    if (tryBlock.HasCaught())
      ReThrowEx(tryBlock);
    
    if (result->BooleanValue())
      NanReturnValue(NanNew<v8::Integer>(i));
  }
  
  NanReturnValue(NanNew<v8::Integer>(-1));
}

/**
 * Return a new xQ instance containing the the first node from this xQ's set
 */
NAN_METHOD(xQWrapper::First) {
  NanScope();
  xQ* out = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  xQStatusCode result = xQ_first(obj->_xq, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the the last node from this xQ's set
 */
NAN_METHOD(xQWrapper::Last) {
  NanScope();
  xQ* out = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  xQStatusCode result = xQ_last(obj->_xq, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return the length/size/count of the xQ instance
 */
NAN_PROPERTY_GETTER(xQWrapper::GetLength) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  NanReturnValue(NanNew<v8::Number>((double)xQ_length(obj->_xq)));
}

/**
 * Return a new xQ instance containing the next immediate sibling of each
 * node in this set, optionally filtered by a selector
 */
NAN_METHOD(xQWrapper::Next) {
  NanScope();
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_next(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the all the next siblings of each
 * node in this set, optionally filtered by a selector
 */
NAN_METHOD(xQWrapper::NextAll) {
  NanScope();
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_nextAll(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing all the next siblings of the nodes
 * in this set up to ones matching the supplied selector
 */
NAN_METHOD(xQWrapper::NextUntil) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_nextUntil(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing all the nodes in this set which
 * do not match the supplied selector
 */
NAN_METHOD(xQWrapper::Not) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_not(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the parent node of each
 * node in this set, optionally filtered by a selector
 */
NAN_METHOD(xQWrapper::Parent) {
  NanScope();
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_parent(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the all the ancestors of each
 * node in this set, optionally filtered by a selector
 */
NAN_METHOD(xQWrapper::Parents) {
  NanScope();
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_parents(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing all the ancestors of the nodes
 * in this set up to ones matching the supplied selector
 */
NAN_METHOD(xQWrapper::ParentsUntil) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_parentsUntil(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the prevous immediate sibling of each
 * node in this set, optionally filtered by a selector
 */
NAN_METHOD(xQWrapper::Prev) {
  NanScope();
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_prev(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing the all the previous siblings of each
 * node in this set, optionally filtered by a selector
 */
NAN_METHOD(xQWrapper::PrevAll) {
  NanScope();
  xmlChar* selectorStr = 0;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());

  if (args.Length() > 0 && !args[0]->IsUndefined() && !args[0]->IsNull())
    selectorStr = (xmlChar*) *selector;

  xQ* out = 0;
  xQStatusCode result = xQ_prevAll(obj->_xq, selectorStr, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return a new xQ instance containing all the previous siblings of the nodes
 * in this set up to ones matching the supplied selector
 */
NAN_METHOD(xQWrapper::PrevUntil) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  v8::String::Utf8Value selector(args[0]->ToString());
  xQ* out = 0;
  
  xQStatusCode result = xQ_prevUntil(obj->_xq, (xmlChar*) *selector, &out);
  assertStatusOK(result);
  
  NanReturnValue(xQWrapper::New(out));
}

/**
 * Return the text content of the first node in the list
 */
NAN_METHOD(xQWrapper::Text) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  xmlChar* txt = xQ_getText(obj->_xq);
  assertPointerValid(txt);
  
  v8::Local<v8::String> retTxt = NanNew<v8::String>((const char*)txt);
  
  xmlFree(txt);
  
  NanReturnValue(retTxt);
}

/**
 * Return the xml content of the first node in the list
 */
NAN_METHOD(xQWrapper::Xml) {
  NanScope();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  assertGotWrapper(obj);
  
  xmlChar* txt = xQ_getXml(obj->_xq);
  assertPointerValid(txt);
  
  v8::Local<v8::String> retTxt = NanNew<v8::String>((const char*)txt);
  
  xmlFree(txt);
  
  NanReturnValue(retTxt);
}



/**
 * Return the value at the given index
 */
NAN_INDEX_GETTER(xQWrapper::GetIndex) {
  NanScope();
  
  v8::Local<v8::Array> list = v8::Local<v8::Array>::Cast(args.This()->GetHiddenValue(NanNew<v8::String>("_nodes")));
  
  NanReturnValue(list->Get(index));
}

/**
 * Return the property attributes of the indexed item
 */
NAN_INDEX_QUERY(xQWrapper::QueryIndex) {
  NanScope();

  v8::Local<v8::Integer> props;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  if (!obj)
    NanReturnValue(props);
  
  uint32_t len = (uint32_t) xQ_length(obj->_xq);
  
  if (index < len)
    props = NanNew<v8::Integer>((int)v8::ReadOnly);

  NanReturnValue(props);
}

/**
 * Disallow setting indexed values
 */
NAN_INDEX_SETTER(xQWrapper::SetIndex) {
  NanScope();
  NanReturnValue(value);
}

/**
 * Disallow deleting values
 */
NAN_INDEX_DELETER(xQWrapper::DeleteIndex) {
  NanScope();
  NanReturnValue(NanFalse());
}

NAN_INDEX_ENUMERATOR(xQWrapper::EnumIndicies) {
  NanScope();
  v8::Local<v8::Array> idxs = NanNew<v8::Array>();
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(args.This());
  if (!obj)
    NanReturnValue(idxs);
  
  uint32_t len = (uint32_t) xQ_length(obj->_xq);
  
  for (uint32_t i = 0; i < len; ++i)
    idxs->Set(i, NanNew<v8::Integer>(i));

  NanReturnValue(idxs);
}
