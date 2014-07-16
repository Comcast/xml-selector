#include <node.h>
#include <libxq.h>

using namespace v8;

//Handle<Value> ParseString(const Arguments& args);

void RegisterModule(Handle<Object> target) {
  /*
  target->Set(String::NewSymbol("parseString"),
    FunctionTemplate::New(ParseString)->GetFunction());
  */
}

NODE_MODULE(xqjs, RegisterModule);
