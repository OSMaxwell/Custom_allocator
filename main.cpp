#include <stdio.h>
#include <deque>
#include <iostream>
#include <vector>
//#include "allocator_test.h"
//#include "allocator_test.cc"
//#include "pretty.cc"
#include "malloc_hook.h"
#include "malloc_hook.cc"
//#include <mcheck.h>

#define M_PI 3.14159265358979323846

// Look at this:
// https://www.gnu.org/software/libc/manual/html_node/Hooks-for-Malloc.html

struct Test {  // Used for testing.
  int m = 0;
  Test(int x) : m(x) { printf("Default test called with m = %i.\n", m); }
  // Test() : m(0) {printf("Default test called.\n");}
  Test(const Test& t) : m(t.m) { printf("Copy test called with m = %i.\n", m); }
};

int main() {
#ifdef _DEBUG
  std::cout << "-----DEBUG MODE---- \n";
  printf("GCC version: %d.%d.%d\n", __GNUC__, __GNUC_MINOR__,
         __GNUC_PATCHLEVEL__);
  // Which version of C++ is being used?
  if (__cplusplus == 201703L)
    std::cout << "Version: C++17" << std::endl;
  else if (__cplusplus == 201402L)
    std::cout << "Version: C++14" << std::endl;
  else if (__cplusplus == 201103L)
    std::cout << "Version: C++11" << std::endl;
  else if (__cplusplus == 199711L)
    std::cout << "Version: C++98" << std::endl;
  else
    std::cout << "pre-standard C++" << std::endl;
#else
  std::cout << "-----RELEASE MODE---- \n\n";
#endif
  // cus_init_hook();
  std::vector<int> v1;
  for (int i = 0; i < 16; i++) {
    v1.push_back(-i);
  }
  for (int i = 0; i < v1.size(); i++) {
    printf("%i ", v1[i]);
  }
/*   for (auto it = global.TagToStaticBuf.cbegin();
       it != global.TagToStaticBuf.cend(); ++it) {
    std::cout << it->first << " " << it->second << "\n";
  } */
  printf("\n--------***OVER***~~------------------------\n");
}