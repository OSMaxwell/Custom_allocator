#ifndef MALLOC_HOOK_H
#define MALLOC_HOOK_H
#include <map>

// Used by GLOBAL
struct ptr_id {
  size_t size;
  int firstUsed;
  int lastUsed;
  int use;
  ptr_id(size_t _size, int _firstUsed)
      : size(_size), firstUsed(_firstUsed), lastUsed(-1),use(0){};
};

// Contains necessary information about Buffer bindings.
struct GLOBAL {
  std::map<void*, ptr_id> StatBufMapper;  // {ADDRESS,{size,firstUsed,lastUsed}}
  int counter = 0;  // A timer-like variable to store allocs and frees
};                  // Global

extern GLOBAL global;

#endif