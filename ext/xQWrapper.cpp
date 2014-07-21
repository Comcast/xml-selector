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
  proto->Set(v8::String::NewSymbol("attr"), v8::FunctionTemplate::New(Attr)->GetFunction());
  proto->Set(v8::String::NewSymbol("forEach"), v8::FunctionTemplate::New(ForEach)->GetFunction());
  proto->Set(v8::String::NewSymbol("find"), v8::FunctionTemplate::New(Find)->GetFunction());
  proto->SetAccessor(v8::String::NewSymbol("length"), GetLength);
  proto->Set(v8::String::NewSymbol("text"), v8::FunctionTemplate::New(Text)->GetFunction());
  
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
  
  if (val->IsArray()) {
    
    v8::TryCatch tryBlock;
    v8::Local<v8::Array> ary = v8::Local<v8::Array>::Cast(val);
    uint32_t len = ary->Length();

    for (uint32_t i = 0; i < len; i++) {
      addToNodeList(q, ary->Get(i));
      
      if (tryBlock.HasCaught())
        return tryBlock.ReThrow();
    }
    
  } else if (val->IsObject()) {
    
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
 * `new xQ(...)`  or just `xQ(...)` in JavaScript
 */
v8::Handle<v8::Value> xQWrapper::New(const v8::Arguments& args) {
  v8::HandleScope scope;
  v8::Handle<v8::Value> inst;
  
  // `new xQ()`
  if (args.IsConstructCall()) {
    
    xQWrapper* obj = new xQWrapper();
    assertPointerValid(obj);
    
    xQStatusCode result = XQ_OK;
    
    if (args.Length() == 0) {

      result = xQ_alloc_init(&(obj->_xq));

    } else if (args.Length() == 1 && args[0]->IsString()) {
      
      v8::String::Utf8Value str(args[0]->ToString());
      xmlDocPtr doc = 0;
      result = xQ_alloc_initMemory(&(obj->_xq), *str, str.length(), &doc);
      
    } else {

      result = xQ_alloc_init(&(obj->_xq));

      if (result == XQ_OK) {
        v8::TryCatch tryBlock;
        int argslen = args.Length();

        for (int argi = 0; argi < argslen; argi++) {
          addToNodeList(obj->_xq, args[argi]);
      
          if (tryBlock.HasCaught()) {
            delete obj;
            return tryBlock.ReThrow();
          }
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
  
  // just `xQ()`
  } else {
    
    // turn this into a `new xQ()` call
    int argc = args.Length();

    if (argc) {
      v8::Local<v8::Value>* argvp = new v8::Local<v8::Value>[argc];
      assertPointerValid(argvp);
    
      for (int i = 0; i < argc; i++) { argvp[i] = args[i]; }

      inst = scope.Close(constructor->NewInstance(argc, argvp));
    
      delete[] argvp;
    } else {
      inst = scope.Close(constructor->NewInstance());
    }
    
    return inst;
    
  }
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
 * Return a new xQ instance containing the nodes that match the provided selector
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
 * Return the length/size/count of the xQ instance
 */
v8::Handle<v8::Value> xQWrapper::GetLength(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(info.This());
  assertGotWrapper(obj);
  
  return scope.Close(v8::Number::New((double)xQ_length(obj->_xq)));
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
