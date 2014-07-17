#include "xQWrapper.h"
#include <xml_node.h>
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
  proto->SetAccessor(v8::String::NewSymbol("length"), GetLength);

  
  // export it
  constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
  exports->Set(v8::String::NewSymbol("xQ"), constructor);
}

/**
 * Destructor
 */
xQWrapper::~xQWrapper() {
  if (_xq)
    xQ_free(_xq, 1);
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
  
  wrapper->Set(v8::String::NewSymbol("_nodes"), list, v8::ReadOnly);
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
    
    xQStatusCode result = xQ_alloc_init(&(obj->_xq));
    
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
    
      for (int i = 0; i < argc; i++) { argvp[i] = args[0]; }

      inst = scope.Close(constructor->NewInstance(argc, argvp));
    
      delete[] argvp;
    } else {
      inst = scope.Close(constructor->NewInstance());
    }
    
    return inst;
    
  }
}

/**
 * Return the length/size/count of the xQ instance
 */
v8::Handle<v8::Value> xQWrapper::GetLength(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
  v8::HandleScope scope;
  
  xQWrapper* obj = node::ObjectWrap::Unwrap<xQWrapper>(info.This());
  
  return scope.Close(v8::Number::New((double)xQ_length(obj->_xq)));
}
