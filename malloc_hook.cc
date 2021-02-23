#include "malloc_hook.h"
#include <malloc.h>  //Just for reference
#include <stdio.h>
#ifdef _DEBUG
#include <ctime>
#endif

// Source:
// https://www.gnu.org/software/libc/manual/html_node/Hooks-for-Malloc.html

GLOBAL global;

/*Pretty prints */
void pretty_header() {
  printf("%-7s%-10s%-35s%-20s\n", "Order", "Operation", "Pointer", "Size");
}
inline void pretty_malloc_print(void *ptr, size_t size) {
  printf("%-7i%-10s%-35p%-20i\n", global.counter, "malloc", ptr, size);
}
inline void pretty_free_print(void *ptr, bool recorded) {
  if (recorded) {
    printf("%-7i%-10s%-35p\n", global.counter, "free", ptr);
  } else {
    printf("%-7i%-10s%-35p%-3s\n", global.counter, "free", ptr, "Not");
  }
}

inline void pretty_map_print(void) {
  printf("%-35s%-10s%-10s%-10s%-5s\n", "Pointer", "Size", "Allocated", "Lifetime","Reuse");
  for (auto elem : global.StatBufMapper) {
    printf("%-35p%-10i%-10i%-10i%-5i\n", elem.first, elem.second.size,
           elem.second.firstUsed, elem.second.lastUsed,elem.second.use);
  }
}

/*prototypes*/
static void cus_hook_init(void);
static void *cus_malloc_hook(size_t, const void *);
static void cus_free_hook(void *, const void *);

// /*original hooks*/
static void *(*old_malloc_hook)(size_t, const void *);
static void (*old_free_hook)(void *, const void *);

// Use this to replace hooks. MANDATORY!
static void cus_hook_init(void) {
  old_malloc_hook = __malloc_hook;
  old_free_hook = __free_hook;
  __malloc_hook = cus_malloc_hook;
  __free_hook = cus_free_hook;
}

static void done_hook(void) {
  __malloc_hook = old_malloc_hook;
  __free_hook = old_free_hook;
}

// A custom hook for malloc
static void *cus_malloc_hook(size_t size, const void *caller) {
  void *result;
  /* Restore all old hooks */
  __malloc_hook = old_malloc_hook;
  __free_hook = old_free_hook;
  /* Call recursively */
  result = malloc(size);
  /* Save underlying hooks */
  old_malloc_hook = __malloc_hook;
  old_free_hook = __free_hook;
  /* printf might call malloc, so protect it too. */
  pretty_malloc_print(result, size);
  // CAN ADD MAP HERE.
  auto it = global.StatBufMapper.find(result);
  if (it == global.StatBufMapper.end()) {
    global.StatBufMapper.insert({result, ptr_id(size,global.counter)});
  } else {
    it->second.size = size;
    it->second.use++;
  }
  /* Restore our own hooks */
  __malloc_hook = cus_malloc_hook;
  __free_hook = cus_free_hook;
  global.counter++;
  return result;
}
// A custom hook for free
static void cus_free_hook(void *ptr, const void *caller) {
  /* Restore all old hooks */
  __malloc_hook = old_malloc_hook;
  __free_hook = old_free_hook;
  /* Call recursively */
  free(ptr);
  /* Save underlying hooks */
  old_malloc_hook = __malloc_hook;
  old_free_hook = __free_hook;
  /* printf might call free, so protect it too. */
  auto it = global.StatBufMapper.find(ptr);
  if (it != global.StatBufMapper.end()) {
    it->second.lastUsed = global.counter;
    pretty_free_print(ptr, true);
  } else {
    pretty_free_print(ptr, false);
  }
  /* Restore our own hooks */
  __malloc_hook = cus_malloc_hook;
  __free_hook = cus_free_hook;
  global.counter++;
}
