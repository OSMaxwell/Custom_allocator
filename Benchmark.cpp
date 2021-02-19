// Compile and test at https://quick-bench.com/
// REQUIREMENTS: - C++17 or newer.
//               - GCC 9.1 or newer.
//               - STL = libstdc++(GNU)
// Bench results found in git.

#include <list>
#include <memory_resource>

static void pmr_list(benchmark::State& state) {
  for (auto _ : state) {
    std::byte stackBuf[2048];
    std::pmr::monotonic_buffer_resource rsrc(stackBuf, sizeof stackBuf);
    std::pmr::list<int> list1(
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, &rsrc);
    std::pmr::list<int> list2(
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, &rsrc);
  }
}
// Benchmark this function
BENCHMARK(pmr_list);

static void RegularList(benchmark::State& state) {
  for (auto _ : state) {
    std::list<int> list3{1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                         1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::list<int> list4{1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                         1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  }
}
BENCHMARK(RegularList);

static void std_vector_nopush(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<int> vec1{1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> vec2{1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  }
}
BENCHMARK(std_vector_nopush);

static void pmr_vector(benchmark::State& state) {
  for (auto _ : state) {
    std::byte stackBuf[2048];
    std::pmr::monotonic_buffer_resource rsrc(stackBuf, sizeof stackBuf);
    std::pmr::vector<int> vec1{
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, &rsrc};
    std::pmr::vector<int> vec2{
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, &rsrc};
  }
}
BENCHMARK(pmr_vector);