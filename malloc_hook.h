#ifndef MALLOC_HOOK_H
#define MALLOC_HOOK_H
//#include <map>

#define ARRAY_SIZE 200 


// Contains necessary information about Buffer to
// container Tag bindings.
struct GLOBAL {
  void* address_table[ARRAY_SIZE];
  size_t size_table[ARRAY_SIZE];
  //std::map<void*, size_t> TagToStaticBuf;  // {ADDRESS,SIZE}
  int tag = 0;
};  // Global

extern GLOBAL global;

#endif