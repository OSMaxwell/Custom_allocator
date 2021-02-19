// Compile and test at https://quick-bench.com/
// REQUIREMENTS: - C++17 or newer.
//               - GCC 9.1 or newer.
//               - STL = libstdc++(GNU)
// Bench results found in git.

#include <list>
#include <vector>
#include <memory_resource> //C++17

/*static void pmr_list(benchmark::State& state){
  for (auto _:state){
    std::byte stackBuf[2048];
    std::pmr::monotonic_buffer_resource rsrc(stackBuf, sizeof stackBuf);
    std::pmr::list<int> list1({1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10},&rsrc);
    std::pmr::list<int> list2({1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10},&rsrc);
  }
}
//Benchmark this function 
BENCHMARK(pmr_list);


static void std_list(benchmark::State& state){
  for (auto _:state){
    std::list<int> list3{1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10};
    std::list<int> list4{1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10};
  }
}
BENCHMARK(std_list); */

/*static void std_vector_nopush(benchmark::State& state){
  for (auto _:state){
    std::vector<int> vec1{1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10};
    std::vector<int> vec2{1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10};
  }
}
BENCHMARK(std_vector_nopush); */

/*static void pmr_vector_nopush(benchmark::State& state){
    for (auto _:state){
      std::byte stackBuf[2048];
      std::pmr::monotonic_buffer_resource rsrc(stackBuf, sizeof stackBuf);
      std::pmr::vector<int> vec1{{1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10},&rsrc};
  }
}
BENCHMARK(pmr_vector_nopush); */

/*static void std_vector_push(benchmark::State& state){
  for (auto _:state){
    std::vector<int> vec1{1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10};
    std::vector<int> vec2{1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10};

    vec1.push_back(10);
    vec1.push_back(10);

    vec1.pop_back();
    vec1.pop_back();


    vec2.push_back(10);
    vec2.push_back(10);

    vec2.pop_back();
    vec2.pop_back();


  }
}
BENCHMARK(std_vector_push); */

/*static void pmr_vector_push(benchmark::State& state){
    for (auto _:state){
      std::byte stackBuf[2048];
      std::pmr::monotonic_buffer_resource rsrc(stackBuf, sizeof stackBuf);
      std::pmr::vector<int> vec1{{1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10},&rsrc};

    vec1.push_back(10);
    vec1.push_back(10);

    vec1.pop_back();
    vec1.pop_back();

  }
}
BENCHMARK(pmr_vector_push); */


/* static void pmr_vector_access(benchmark::State& state){
    for (auto _:state){
      std::byte stackBuf[2048];
      std::pmr::monotonic_buffer_resource rsrc(stackBuf, sizeof stackBuf);
      std::pmr::vector<int> vec1{{1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10},&rsrc};
      vec1[5]= 20;

  }
}
BENCHMARK(pmr_vector_access);  */

/* static void std_vector_access(benchmark::State& state){
  for (auto _:state){
    std::vector<int> vec1{1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10};
    vec1[5]= 20;
  }
}
BENCHMARK(std_vector_access);  */