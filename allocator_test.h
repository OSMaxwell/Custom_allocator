#ifndef ALLOCATOR_TEST_H
#define ALLOCATOR_TEST_H
#include <map>
//#include "tensorflow/lite/c/common.h"

namespace tflite {
namespace ops {
namespace micro {

// Whenever  a new vector is created, a new copy, a second allocate is
// constructed without the first having been destructed-> then we need a new
// buffer. --- Use with use ( increase with alloc, decrease with deall)

// Contains necessary information about the TFLiteContext and Buffer to
// container Tag bindings.
struct GLOBAL {
  // TfLiteContext ctx;
  // int TagIncrement = 0; --> Not needed now
  std::map<int, std::pair<void*, size_t>> TagToStaticBuf; // {TAG,{STATIC BUFFER,SIZE}} mapping. DO NOT USE DIRECTLY
  std::map<int, int> TagToBufferCounter ;
  //int BufferCounter = 0 ;
};  // Global

extern GLOBAL global;

template <typename T>
void InitTagToStaticBuf(int tag, T* buff_ptr, size_t size);

}  // namespace micro
}  // namespace ops
}  // namespace tflite
#endif