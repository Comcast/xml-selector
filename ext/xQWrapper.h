#ifndef __XQWRAPPER_H_INCLUDED__
#define __XQWRAPPER_H_INCLUDED__

#include <node.h>
#include <libxq.h>

class xQWrapper : public node::ObjectWrap {
public:
  static void Init(v8::Handle<v8::Object> exports);
  
  static v8::Local<v8::Object> New(xQ* xq);

protected:
  xQWrapper() : _xq(0) { };
  xQWrapper(xQ* val) : _xq(val) { };
  ~xQWrapper();
  
  void shadowNodeList(v8::Local<v8::Object> wrapper);
  
  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Attr(const v8::Arguments& args);
  static v8::Handle<v8::Value> Children(const v8::Arguments& args);
  static v8::Handle<v8::Value> Closest(const v8::Arguments& args);
  static v8::Handle<v8::Value> Filter(const v8::Arguments& args);
  static v8::Handle<v8::Value> Find(const v8::Arguments& args);
  static v8::Handle<v8::Value> First(const v8::Arguments& args);
  static v8::Handle<v8::Value> ForEach(const v8::Arguments& args);
  static v8::Handle<v8::Value> Last(const v8::Arguments& args);
  static v8::Handle<v8::Value> GetLength(v8::Local<v8::String> property, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> Next(const v8::Arguments& args);
  static v8::Handle<v8::Value> NextAll(const v8::Arguments& args);
  static v8::Handle<v8::Value> NextUntil(const v8::Arguments& args);
  static v8::Handle<v8::Value> Prev(const v8::Arguments& args);
  static v8::Handle<v8::Value> PrevAll(const v8::Arguments& args);
  static v8::Handle<v8::Value> PrevUntil(const v8::Arguments& args);
  static v8::Handle<v8::Value> Text(const v8::Arguments& args);
  static v8::Handle<v8::Value> Xml(const v8::Arguments& args);
  
  static v8::Handle<v8::Value>   GetIndex(uint32_t index, const v8::AccessorInfo& info);
  static v8::Handle<v8::Integer> QueryIndex(uint32_t index, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value>   SetIndex(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
  static v8::Handle<v8::Boolean> DeleteIndex(uint32_t index, const v8::AccessorInfo& info);
  static v8::Handle<v8::Array>   EnumIndicies(const v8::AccessorInfo& info);
  
  static v8::Persistent<v8::Function> constructor;
  
  xQ* _xq;
};

#endif // __XQWRAPPER_H_INCLUDED__
