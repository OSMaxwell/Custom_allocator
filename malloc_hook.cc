#include "malloc_hook.h"
//#include <malloc.h>  //Just for reference
//#include <stdint.h>
//#include <stdio.h>
//#include <stdlib.h>
/* #ifdef _DEBUG
#include <ctime>
#endif */

#define MALLOC_HOOK_VERSION "1.0.0"

// Source:
// https://www.gnu.org/software/libc/manual/html_node/Hooks-for-Malloc.html

GLOBAL global;

extern void *__libc_malloc(size_t size);

//--- Thread safe implementation ----

int new_malloc_hook_active = 0;

void *new_malloc_hook(size_t size, void *caller) {
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
void *malloc(size_t size) {
  printf("called");
  void *caller = __builtin_return_address(0);
  if (new_malloc_hook_active) {
    return new_malloc_hook(size, caller);
  }
  return __libc_malloc(size);
}
void happen(void) { printf("happen"); }

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
