#include <node.h>
#include <libxq.h>
#include "xQWrapper.h"

using namespace v8;

void RegisterModule(Handle<Object> target) {
  xQWrapper::Init(target);
}

NODE_MODULE(xqjs, RegisterModule);
