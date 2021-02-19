# Description
**A new branch for PMR Implementation and research.**

# Research and Comparision 
## std::pmr::polymorphic_allocator (C++17) vs std:: (Pre C++17)

### Memory resources <memory_resource> under std::pmr (C++17)
Definitions and resources can be found [here](https://en.cppreference.com/w/cpp/memory).

| Class Name  | Desciprtion  |
| ------------- | ------------- |
| memory_resource  | an abstract interface for classes that encapsulate memory resources   |
| synchronized_pool_resource | a thread-safe std::pmr::memory_resource for managing allocations in pools of different block sizes   |
| unsynchronized_pool_resource | a thread-unsafe std::pmr::memory_resource for managing allocations in pools of different block sizes |
| monotonic_buffer_resource | a special-purpose std::pmr::memory_resource that releases the allocated memory only when the resource is destroyed | 

The goal is to understand these, test them and try to create the custom static buffer allocator for our use case.

### Benchmarks 
Results can also be found at https://quick-bench.com/ from Benchmark.cpp 
Uncomment/Comment to bench.
_PS: Not all functions can be combined for benching at the same time._

**To compile C++17 + GCC 9.1 are _required_.**

![PMR vs RegularSTD](pic/90X72XHGO6fFbP1xra40SyHJlbw.png)

monotonic_buffer_resource is noticeably faster !

![pmr_vec vs std_vec](pic/OVFVr-k-fuFDTCHj40VziBIjJWU.png)
TEST CASE: Create two vectors using both pmr and regular std. 

You guessed it, pmr is faster. :+1:

![push_back effect](pic/qdWLmSq97DgnKT5dbEs8QdAac44.png)
```
    //Create a regular vector / pmr vector
    ...
    
    vec1.push_back(10);
    vec1.push_back(10);

    vec1.pop_back();
    vec1.pop_back();
```
... and its effect on speed.

