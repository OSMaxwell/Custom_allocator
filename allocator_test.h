#ifndef ALLOCATOR_TEST_H
#define ALLOCATOR_TEST_H
#include <map>
//#include "tensorflow/lite/c/common.h"

namespace tflite {
namespace ops {
namespace micro {
// Contains necessary information about the TFLiteContext and Buffer to
// container Tag bindings.
struct GLOBAL {
  std::map<int, std::pair<void*, size_t>> TagToStaticBuf; // {TAG,{STATIC BUFFER,SIZE}} mapping. DO NOT USE DIRECTLY
  std::map<int, int> TagToBufferCounter ;
};  // Global

extern GLOBAL global;

template <typename T>
void InitTagToStaticBuf(int tag, T* buff_ptr, size_t size);

}  // namespace micro
}  // namespace ops
}  // namespace tflite
#endif