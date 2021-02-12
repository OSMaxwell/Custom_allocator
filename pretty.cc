// Output functions for pretty printing related to STL Containers and custom
// allocators.
#include <stdio.h>
#include "allocator_test.h"
//#include "allocator_test.cc"

// @brief Print header table for future allocation.
void pretty_header() {
  printf("%-7s%-35s%-20s%-10s%-5s\n", "Tag", "Operation", "Pointer", "Size",
         "Cntr");
}

// @brief Print the name of a function respecting the @a pretty_header layout.
// @param op_name : Name of the function to start
void pretty_op_start(const char *op_name) {
  printf("\nStarting:");
  printf("%20s\n", op_name);
}

// @brief Outputs stats/characters of a vector after creation
// @param vec : The specified vector (Only use vectors that are using
// ArenaBufferAllocator)
template <class T>
void pretty_stats(
    std::vector<T, tflite::ops::micro::ArenaBufferAllocator<T>> &vec) {
  printf(
      "-----------------------------------------------------------------------"
      "-\n");
  printf("%d: Capacity= %d|", tag, vec.capacity());
  printf("Max_size= %d|", vec.max_size());
  printf("Size= %d\n", vec.size());
  printf(
      "-----------------------------------------------------------------------"
      "-\n");
}

// @brief Outputs a given STL vector and its content. At the end, a summary of
// its stats is also given.
// @param vec : The specified vector (Use for normal STL vectors)
template <class T>
void pretty_print(std::vector<T> vec) {
  printf("\n i    |vec[i]|\n");
  if (vec.size() == 0) {
    printf("Empty\n");
  }
  for (int i = 0; i < vec.size(); i++) {
    printf("%-6d|%-6d|\n", i, vec[i]);
  }
  printf(
      "-----------------------------------------------------------------------"
      "-\n");
  printf("Capacity= %d|", vec.capacity());
  printf("Max_size= %d|", vec.max_size());
  printf("Size= %d\n", vec.size());
  printf(
      "-----------------------------------------------------------------------"
      "-\n");
}
// @brief Outputs a given custom allocated vector and its content. At the end, a
// summary of its stats is also given.
// @param vec : The specified vector (Only use vectors with
// ArenaBufferAllocator)
template <class T>
void pretty_print(
    std::vector<T, tflite::ops::micro::ArenaBufferAllocator<T>> &vec) {
  printf("\n i    |vec[i]|\n");
  if (vec.size() == 0) {
    printf("\tEmpty\n");
  }
  if (std::is_floating_point<T>::value) {
    for (int i = 0; i < vec.size(); i++) {
      printf("%-6d|%-6f|\n", i, vec[i]);
    }
  } else {
    for (int i = 0; i < vec.size(); i++) {
      printf("%-6d|%-6d|\n", i, vec[i]);
    }
  }
  pretty_stats(vec);
}

//@brief Prints the Static to Buffer Mapping that has been already stored.
void pretty_mapping() {
  printf("\n~~~~~~~~~~~MAPPING~~~~~~~~~~\n");
  printf("%-7s|%-19s|%-9s|%-9s\n", "Tag", "Buffer_Pointer", "Size","Counter");
  for (auto it = tflite::ops::micro::global.TagToStaticBuf.cbegin();
       it != tflite::ops::micro::global.TagToStaticBuf.cend(); it++) {
    printf("%-8d%-20p%-10d%-10d\n", it->first, it->second._static_buffer, it->second._size,it->second._counter);
  }
  printf("\n");
}