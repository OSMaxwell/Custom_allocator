#ifndef ALLOCATOR_TEST_H
#define ALLOCATOR_TEST_H
#include <map>
//#include "tensorflow/lite/c/common.h"

namespace tflite {
namespace ops {
namespace micro {

// Used by GLOBAL to contain information about the tag.
struct tag_id {
  void* _static_buffer;
  std::size_t _size;
  int _counter;
  tag_id(void* static_buffer, std::size_t size)
      : _static_buffer(static_buffer), _size(size),_counter(0) {};
};

// Contains necessary information about Buffer to
// container Tag bindings.
struct GLOBAL {
  // {TAG,{STATIC BUFFER,SIZE,IN_USE,COUNTER}} mapping. DO NOT WRITE INTO DIRECTLY!
  std::map<int, tag_id> TagToStaticBuf;  
  int tag = 0;
};  // Global

extern GLOBAL global;

template <typename T>
void InitTagToStaticBuf(T* buff_ptr, size_t size);

}  // namespace micro
}  // namespace ops
}  // namespace tflite
#endif