#ifndef __XQWRAPPER_H_INCLUDED__
#define __XQWRAPPER_H_INCLUDED__

#include <node.h>
#include <libxq.h>

class xQWrapper : public node::ObjectWrap {
public:
  static void Init(v8::Handle<v8::Object> exports);

protected:
  xQWrapper() : _xq(0) { };
  xQWrapper(xQ* val) : _xq(val) { };
  ~xQWrapper();
  
  void shadowNodeList(v8::Local<v8::Object> wrapper);
  
  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> GetLength(v8::Local<v8::String> property, const v8::AccessorInfo& info);
  
  static v8::Persistent<v8::Function> constructor;
  
  xQ* _xq;
};

#endif // __XQWRAPPER_H_INCLUDED__
