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
#ifndef __XMLSELECTOR_UTILS_H_INCLUDED__
#define __XMLSELECTOR_UTILS_H_INCLUDED__

#if (NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION)
#define ThrowEx(msg) do { NanThrowError(msg); return; } while (0);
#define ReThrowEx(blk) do { blk.ReThrow(); return; } while (0);
#else
#define ThrowEx(msg) NanThrowError(msg)
#define ReThrow(blk) return blk.ReThrow();
#endif

#define assertPointerValid(ptr) \
  if (!(ptr)) ThrowEx("Out of memory");

#define assertGotWrapper(ptr) \
  if (!(ptr)) ThrowEx("Method invoked in an incorrect context");

#define FUNCTION_VALUE(f) \
  NanNew<v8::FunctionTemplate>(f)->GetFunction()

#endif // __XMLSELECTOR_UTILS_H_INCLUDED__
