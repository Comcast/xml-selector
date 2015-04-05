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
#ifndef __XQWRAPPER_H_INCLUDED__
#define __XQWRAPPER_H_INCLUDED__

#include <node.h>
#include <nan.h>
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
  
  static NAN_METHOD(New);
  static NAN_METHOD(AddNamespace);
  static NAN_METHOD(Attr);
  static NAN_METHOD(Children);
  static NAN_METHOD(Closest);
  static NAN_METHOD(Filter);
  static NAN_METHOD(Find);
  static NAN_METHOD(FindIndex);
  static NAN_METHOD(First);
  static NAN_METHOD(ForEach);
  static NAN_METHOD(Last);
  static NAN_PROPERTY_GETTER(GetLength);
  static NAN_METHOD(Next);
  static NAN_METHOD(NextAll);
  static NAN_METHOD(NextUntil);
  static NAN_METHOD(Not);
  static NAN_METHOD(Parent);
  static NAN_METHOD(Parents);
  static NAN_METHOD(ParentsUntil);
  static NAN_METHOD(Prev);
  static NAN_METHOD(PrevAll);
  static NAN_METHOD(PrevUntil);
  static NAN_METHOD(Text);
  static NAN_METHOD(Xml);
  
  static NAN_INDEX_GETTER(GetIndex);
  static NAN_INDEX_QUERY(QueryIndex);
  static NAN_INDEX_SETTER(SetIndex);
  static NAN_INDEX_DELETER(DeleteIndex);
  static NAN_INDEX_ENUMERATOR(EnumIndicies);
  
  static v8::Persistent<v8::Function> constructor;
  
  xQ* _xq;
};

#endif // __XQWRAPPER_H_INCLUDED__
