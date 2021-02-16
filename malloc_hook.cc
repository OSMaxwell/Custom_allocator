#include "malloc_hook.h"
//#include <malloc.h>  //Just for reference
#include <stdio.h>
#ifdef _DEBUG
#include <ctime>
#endif

// Source:
// https://www.gnu.org/software/libc/manual/html_node/Hooks-for-Malloc.html

GLOBAL global;

/*prototypes*/
// static void cus_init_hook(void);
// static void *cus_malloc_hook(size_t, const void *);
// static void cus_free_hook(void *, const void *);

// /*original hooks*/
// static void *(*old_malloc_hook)(size_t, const void *);
// static void (*old_free_hook)(void *, const void *);

// // Use this to replace hooks.
// static void cus_init_hook(void) {
//   old_malloc_hook = __malloc_hook;
//   old_free_hook = __free_hook;
//   __malloc_hook = cus_malloc_hook;
//   __free_hook = cus_free_hook;
// }

// // A custom hook for malloc
// static void *cus_malloc_hook(size_t size, const void *caller) {
//   void *result;
//   /* Restore all old hooks */
//   __malloc_hook = old_malloc_hook;
//   __free_hook = old_free_hook;
//   /* Call recursively */
//   result = malloc(size);
//   /* Save underlying hooks */
//   old_malloc_hook = __malloc_hook;
//   old_free_hook = __free_hook;
//   /* printf might call malloc, so protect it too. */
//   printf("Malloc (%i) returns %p\n", size, result);
//   /* Restore our own hooks */
//   __malloc_hook = cus_malloc_hook;
//   __free_hook = cus_free_hook;
//   // global.TagToStaticBuf.insert({result,size}); <- This call calls for more
//   // malloc which calls for map elements!!!
//   global.address_table[global.tag] = result;
//   global.size_table[global.tag] = size;
//   global.tag++;
//   return result;
// }
// // A custom hook for free
// static void cus_free_hook(void *ptr, const void *caller) {
//   /* Restore all old hooks */
//   __malloc_hook = old_malloc_hook;
//   __free_hook = old_free_hook;
//   /* Call recursively */
//   free(ptr);
//   /* Save underlying hooks */
//   old_malloc_hook = __malloc_hook;
//   old_free_hook = __free_hook;
//   /* printf might call free, so protect it too. */
//   printf("freed pointer %p\n", ptr);
//   /* Restore our own hooks */
//   __malloc_hook = cus_malloc_hook;
//   __free_hook = cus_free_hook;
// }

//--- Thread safe implementation ----

int new_malloc_hook_active = 0;

void *new_malloc_hook(std::size_t size, void *caller) {
  void *result;

  // deactivate hooks for logging
  new_malloc_hook_active = 0;

  result = malloc(size);

  // do logging
  printf("Malloc (%i) returns %p\n", size, result);
  global.TagToStaticBuf.insert({result, size});  // Store in map

  // reactivate hooks
  new_malloc_hook_active = 1;

  return result;
}

// Have a look at this :
// https://www.slideshare.net/tetsu.koba/tips-of-malloc-free
void *malloc(std::size_t size) {
  void *caller = __builtin_return_address(0);
  if (new_malloc_hook_active) {
    return new_malloc_hook(size, caller);
  }
  return __libc_malloc(size);
}

// Create also for free.

/*Create a logging function with fprintf()*/
/* FILE *f =fopen ("log.txt",)



This is quite handy, just plug into e.g. some common header file to be called
from anywhere in the program (better approach would be to form a class with
these functions)

inline string getCurrentDateTime( string s ){
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80];
    tstruct = *localtime(&now);
    if(s=="now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if(s=="date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return string(buf);
};
inline void Logger( string logMsg ){

    string filePath = "/somedir/log_"+getCurrentDateTime("date")+".txt";
    string now = getCurrentDateTime("now");
    ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app );
    ofs << now << '\t' << logMsg << '\n';
    ofs.close();
} */
