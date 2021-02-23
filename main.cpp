#include <assert.h>
#include <stdio.h>
#include <deque>
#include <iostream>
#include <vector>
#include "malloc_hook.cc"
#include "malloc_hook.h"

#define M_PI 3.14159265358979323846

// Look at this:
// https://www.gnu.org/software/libc/manual/html_node/Hooks-for-Malloc.html

struct Test {  // Used for testing.
  int m = 0;
  Test(int x) : m(x) { printf("Default test called with m = %i.\n", m); }
  // Test() : m(0) {printf("Default test called.\n");}
  Test(const Test& t) : m(t.m) { printf("Copy test called with m = %i.\n", m); }
};

void case_one_vec() {
  cus_hook_init();
  std::vector<double> v1;
  for (int i = 0; i < 16; i++) {
    v1.push_back(-i);
  }
  printf("Vector size : %i\n", v1.size());
  for (int i = 0; i < v1.size(); i++) {
    printf("%i ", v1[i]);
  }
  printf("\n");
}

void case_vec_in_vec() {
  int col = 1;
  int row = 6;
  int num = 0;
  cus_hook_init();
  std::vector<std::vector<int>> v2;
  for (int j = 0; j < row; j++) {
    std::vector<int> v1;
    for (int j = 0; j < col; j++) {
      v1.push_back(num);
      num += 2;
    }
    v2.push_back(v1);
    col++;
    printf("Outer vector size : %i\n", v2.size());
  }

  for (int i = 0; i < v2.size(); i++) {
    for (int j = 0; j < v2[i].size(); j++) std::cout << v2[i][j] << " ";
    std::cout << std::endl;
  }
}

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
  case_one_vec();

  // final print
  /*   for (int i = 0; i < global.tag; i++) {
      printf("%p,%d\n", global.address_table[i], global.size_table[i]);
    }
    printf("This is : %p", *global.address_table);  // Hmm???????.... */
  pretty_map_print();
  printf("\n--------***OVER***~~------------------------\n");
  //done_hook();
}