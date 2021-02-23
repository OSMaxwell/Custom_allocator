#ifndef MALLOC_HOOK_H
#define MALLOC_HOOK_H
#include <map>

// Contains necessary information about Buffer bindings.
struct GLOBAL {
  std::multimap<void*, std::pair<size_t,int>> StatBufMapper;  // {ADDRESS,{SIZE,LIFETIME}}
  int counter = 0;  // A timer-like variable to store allocs and frees
};                  // Global

extern GLOBAL global;

#endif