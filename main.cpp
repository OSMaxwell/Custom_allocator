#include <stdio.h>
#include <deque>
#include <iostream>
#include <vector>
#include "allocator_test.cc"
#include "allocator_test.h"
#include "pretty.cc"

#define M_PI 3.14159265358979323846

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

  using inner_t =
      std::vector<int, tflite::ops::micro::ArenaBufferAllocator<int>>;
  void* buffer_inner = malloc(400 * sizeof(int));
  void* buffer_outer = malloc(500 * sizeof(inner_t));
  tflite::ops::micro::InitTagToStaticBuf(buffer_inner, 50);
  tflite::ops::micro::InitTagToStaticBuf(buffer_outer, 5);
  pretty_mapping();
  pretty_header();
  std::vector<inner_t, tflite::ops::micro::ArenaBufferAllocator<inner_t>>
      vec_2d;
  pretty_op_start(".push_back inner_t");

  for (int i = 0; i < 5; i++) {
    vec_2d.push_back(inner_t());
    for (int c = 0; c < 4; c++) {
      vec_2d[i].push_back(c);
    }
  }

  for (int k = 0; vec_2d.size(); k++) {
    for (int c = 0; c < 50; c++) {
    std::cout << vec_2d[k][c] << ",";}
  }
  std::cout << "\n";

  printf("--------***OVER***~~------------------------\n");
}