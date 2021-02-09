#include <stdio.h>
#include <deque>
#include <iostream>
#include <vector>
#include "allocator_test.cc"
#include "allocator_test.h"
#include "pretty.cc"

#define M_PI 3.14159265358979323846

struct Test {
  int m = 0;
  Test(int x) : m(x) { printf("Default test called with m = %i.\n", m); }
  // Test() : m(0) {printf("Default test called.\n");}
  Test(const Test& t) : m(t.m) { printf("Copy test called with m = %i.\n", m); }
};

// Try with construct() in traits

int main() {
#ifdef _DEBUG
  std::cout << "-----DEBUG MODE---- \n\n";
#else
  std::cout << "-----RELEASE MODE---- \n\n";
#endif
  // tflite::ops::micro::GLOBAL global;
  /*   pretty_op_start(".push_back");
    std::vector<int> vec1;
    for (int i = 0; i < 16; i++) {
      vec1.push_back(i << 1);
      printf("Size--> %d\n", vec1.size());
    }
    pretty_print<int>(vec1);
    pretty_op_start(".pop_back");
    for (int i = 0; i < 16; i++) {
      vec1.pop_back();
      printf("Size--> %d\n", vec1.size());
    }
    pretty_print<int>(vec1);
   */
  std::cout << "\n~~~~~~~~~~USING CUSTOM ALLOCATOR~~~~~~~ \n\n";
  int buffer[20] = {0};
  int buffer1[20] = {1};
  pretty_header();
  std::vector<int, tflite::ops::micro::ArenaBufferAllocator<int, 12>> vec2(
      tflite::ops::micro::ArenaBufferAllocator<int, 12>(&buffer[0], 20));
  pretty_op_start(".push_back");
  pretty_header();
  for (int i = 0; i < 20; i++) {
    vec2.push_back(77);
  }
  pretty_print(vec2);

  printf("\n~~~~~~~~~~~~END Vec2~~~~~~~~~~~~\n\n");

  std::vector<int, tflite::ops::micro::ArenaBufferAllocator<int, 45>> vec3(
      tflite::ops::micro::ArenaBufferAllocator<int, 45>(&buffer1[0], 20));
  pretty_op_start(".push_back");
  for (int i = 0; i < 5; i++) {
    vec3.push_back(123);
  }
  pretty_print(vec3);
  pretty_op_start(".pop_back");
  for (int i = 0; i < 3; i++) {
    vec3.pop_back();
  }
  pretty_print(vec3);
  printf("\n~~~~~~~~~~~~END Vec3~~~~~~~~~~~~\n\n");

  float buffer2[15];
  tflite::ops::micro::InitTagToStaticBuf<float>(32, &buffer2[0], 15);
  std::vector<float, tflite::ops::micro::ArenaBufferAllocator<float, 32>> vec4(
      tflite::ops::micro::ArenaBufferAllocator<float, 32>(&buffer2[0], 15));
  pretty_op_start(".push_back");
  pretty_header();
  for (int i = 0; i < 9; i++) {
    vec4.push_back(M_PI + i);
  }
  pretty_print(vec4);
  std::cout << "vec4 IS EMPTY? " << vec4.empty() << std::endl;
  vec4.resize(4);
  pretty_print(vec4);
  pretty_op_start(".assign");
  vec4.assign(11, 1.0);
  pretty_print(vec4);
  /*   vec4.clear();
    pretty_print(vec4); */
  auto input_it = vec4.begin();
  int input_remaining = vec4.end() - input_it;
  printf("\n");
  printf("Begin: %p\n", input_it);
  printf("end - begin : %d\n", input_remaining);
  printf("\n");

  printf("\n~~~~~~~~~~~~END Vec4~~~~~~~~~~~~\n\n");
  /*   for (auto it = tflite::ops::micro::global.TagToStaticBuf.cbegin();
         it != tflite::ops::micro::global.TagToStaticBuf.cend(); it++) {
      std::cout << "MAP:" << it->first << " " << it->second.first << "\n";
    } */
  pretty_mapping();
  /*   double buffer5[600];
    std::deque<double, tflite::ops::micro::ArenaBufferAllocator<double, 2>> q(
        tflite::ops::micro::ArenaBufferAllocator<double, 2>(&buffer5[0], 300));
    pretty_op_start(".queue.push_back");
    for (int i = 0; i < 30; i++) {
      q.push_back(2 * i);
    }
    for (int i = 0; i < q.size(); i++) {
      printf("%-6d|%-7f|\n", i, q[i]);
    }
    printf("Size: %d\n", q.size());
    printf("max Size: %d\n", q.max_size());

    pretty_op_start(".queue.shrink_to_fit");
    q.shrink_to_fit();
    pretty_header();
    printf("max Size: %d\n", q.max_size());
    printf("Begin: %p\n", q.begin());  // Check this
    printf("End: %p\n", q.end());      // Check this
    printf("Front: %p\n", q.front()); */

  float buffer6[15];

  tflite::ops::micro::InitTagToStaticBuf(5, &buffer6[0], 15);
  pretty_mapping();
  pretty_header();
  std::vector<float, tflite::ops::micro::ArenaBufferAllocator<float, 5>> vec6;
  vec6.push_back(30);
  vec6.push_back(30);
  vec6.push_back(30);
  vec6.push_back(30);
  vec6.push_back(30);
  vec6.push_back(30);

  pretty_print(vec6);

  using inner_t =  // Use this to avoid long commands
      std::vector<int, tflite::ops::micro::ArenaBufferAllocator<int, 1>>;
  void* buffer_inner;
  buffer_inner = malloc(400 * sizeof(int));
  void* buffer_outer;
  buffer_outer = malloc(500 * sizeof(inner_t));
  tflite::ops::micro::InitTagToStaticBuf(1, buffer_inner, 50);

  std::cout << sizeof(inner_t) << std::endl;
  tflite::ops::micro::InitTagToStaticBuf(6, buffer_outer, 500);
  /*  printf("FROM %p\n",buffer_outer);
   printf("OVERHERE %p",buffer_outer+50); */

  pretty_mapping();
  pretty_header();
  std::vector<inner_t, tflite::ops::micro::ArenaBufferAllocator<inner_t, 6>>
      vec_2d;
  pretty_op_start(".push_back inner_t");
  vec_2d.push_back(inner_t());
  // printf("Data0 %p\n", vec_2d[0].data());
  pretty_op_start("[0].push_back");
  for (int i = 0; i < 3; i++) {
    vec_2d[0].push_back(35);
  }
  // int* row = &vec_2d[0][0];
  printf("Data0 %p\n", vec_2d[0].data());
  printf("Inner %p\n", buffer_inner);
  printf("Test[0][1] %d\n", vec_2d[0][1]);
  pretty_op_start(".push_back inner_t");
  vec_2d.push_back(inner_t());
  printf("Data0 %p\n", vec_2d[0].data());
  pretty_op_start("[1].push_back");
  printf("Data %p\n", vec_2d[0].data());
  for (int i = 0; i < 3; i++) {
    vec_2d[1].push_back(335);
  }
  pretty_op_start(".push_back inner_t");
  vec_2d.push_back(inner_t());
  printf("Data0 %p\n", vec_2d[0].data());
  pretty_op_start("[2].push_back");
  for (int i = 0; i < 3; i++) {
    vec_2d[2].push_back(97);
  }
  pretty_op_start(".push_back inner_t");
  vec_2d.push_back(inner_t());
  pretty_op_start("[3].push_back");
  for (int i = 0; i < 3; i++) {
    vec_2d[3].push_back(54);
  }
  printf("Data0 %p\n", vec_2d[0].data());
  printf("Data1 %p\n", vec_2d[1].data());
  printf("Data2 %p\n", vec_2d[2].data());

  /*
  pretty_op_start(".push_back inner_t");
  vec_2d.push_back(inner_t());
  pretty_op_start("[4].push_back");
    for (int i = 0; i < 3; i++) {
      vec_2d[4].push_back(576);
    } */
  // vec_2d.
  // printf("Hello: %d\n", row[0]);
  // printf("Hello2:  %d\n", row[1]);
  // printf("Test %d\n",vec_2d[0][1]);
  std::cout << "[2][0]:" << vec_2d[2][0] << "in " << &vec_2d[2][0] << std::endl;
  // printf("Woah%d",vec_2d[0][0]);
  printf("[1][1] : %i in %p\n", vec_2d[1][1], &vec_2d[1][1]);
  printf("[3][0] : %i in %p\n", vec_2d[3][0], &vec_2d[3][0]);
  // printf("[4][2] : %i\n", vec_2d[4][2]);
  // printf("[0][0] : %i\n",vec_2d[0][0]);

  pretty_stats(vec_2d[0]);

  printf("-----------------------------------\n");
  void* TestBuffer;
  TestBuffer = malloc(5 * sizeof(Test));
  tflite::ops::micro::InitTagToStaticBuf(75, TestBuffer, 5);
  std::vector<Test, tflite::ops::micro::ArenaBufferAllocator<Test, 75>>
      vec_test;

  vec_test.push_back(Test(7));
  printf("--------******~~------------------------\n");
  vec_test.push_back(Test(8));
  vec_test.push_back(Test(9));
  vec_test.push_back(Test(2));

  printf("%i\n", vec_test[0].m);
  printf("%i\n", vec_test[1].m);
  printf("%i\n", vec_test[2].m);
  printf("%i\n", vec_test[3].m);
  
  printf("--------***OVER***~~------------------------\n");
}