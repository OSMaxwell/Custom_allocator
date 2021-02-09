#include <stdio.h>
#include <deque>
#include <iostream>
#include <vector>
#include "allocator_test.cc"
#include "allocator_test.h"
#include "pretty.cc"
#define M_PI 3.14159265358979323846

struct Test { //Used for testing.
  int m = 0;
  Test(int x) : m(x) { printf("Default test called with m = %i.\n", m); }
  // Test() : m(0) {printf("Default test called.\n");}
  Test(const Test& t) : m(t.m) { printf("Copy test called with m = %i.\n", m); }
};

int main() {
#ifdef _DEBUG
  std::cout << "-----DEBUG MODE---- \n\n";
#else
  std::cout << "-----RELEASE MODE---- \n\n";
#endif
  using inner_t =
      std::vector<int, tflite::ops::micro::ArenaBufferAllocator<int, 1>>;
  void* buffer_inner = malloc(400 * sizeof(int));
  void* buffer_outer = malloc(500 * sizeof(inner_t));
  tflite::ops::micro::InitTagToStaticBuf(1, buffer_inner, 50);
  tflite::ops::micro::InitTagToStaticBuf(6, buffer_outer, 500);
  pretty_mapping();
  pretty_header();
  std::vector<inner_t, tflite::ops::micro::ArenaBufferAllocator<inner_t, 6>>
      vec_2d;
  pretty_op_start(".push_back inner_t");

  for (int i = 0; i < 10; i++) {
    vec_2d.push_back(inner_t());
    for (int c = 0; c < 50; c++) {
      vec_2d[i].push_back(c);
    }
  }

  for (int k = 0; vec_2d.size(); k++) {
    std::cout << vec_2d[k].size() << ",";
  }
  std::cout << "\n";

  printf("--------***OVER***~~------------------------\n");
}