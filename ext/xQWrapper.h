#ifndef __XQWRAPPER_H_INCLUDED__
#define __XQWRAPPER_H_INCLUDED__

#include <node.h>
#include <libxq.h>

class xQWrapper : public node::ObjectWrap {
public:
  static void Init(v8::Handle<v8::Object> exports);

private:
  xQWrapper() : _xq(0) { };
  xQWrapper(xQ* val) : _xq(val) { };
  ~xQWrapper();
  
  static v8::Handle<v8::Value> New(const v8::Arguments& args);

  static v8::Persistent<v8::Function> constructor;
  
  xQ* _xq;
};

#endif // __XQWRAPPER_H_INCLUDED__
