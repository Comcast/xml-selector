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
#ifndef __XMLSELECTOR_ELEMENT_H_INCLUDED__
#define __XMLSELECTOR_ELEMENT_H_INCLUDED__

#include "Node.h"

namespace xmlselector {

class Element : public Node {
public:
  static void Init(v8::Handle<v8::Object> exports);

  static v8::Persistent<v8::Function> constructor;

  xmlElementPtr elem() { return (xmlElementPtr) _node; }

protected:

  explicit Element(xmlElementPtr elem);
  virtual ~Element();
  
  static NAN_METHOD(New);
  static NAN_METHOD(GetAttribute);
  
  void elem(xmlElementPtr newElem) { node((xmlNodePtr) newElem); }

};

} // namespace xmlselector

#endif // __XMLSELECTOR_ELEMENT_H_INCLUDED__