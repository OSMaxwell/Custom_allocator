#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <scoped_allocator>
#include <assert.h>
#include <string>
#include <memory_resource>

void scenario0()
{
  int buffer[100] = {0}; // this buffer hold 100 integers.
  std::pmr::monotonic_buffer_resource pool{std::data(buffer), std::size(buffer)};
  std::pmr::vector<int> v(&pool);
  assert(v.size() == 0);
  for (int i = 0; i < 50; i++)
  {
    v.push_back(i);
  }

  assert(v.size() == 50);

  for (int i = 0; i < 100; i++)
  {
    try
    {
      v.push_back(i);
    }
    catch (const std::exception &e)
    {
      std::cout << "Caught an expected buffer overflow error";
    }
  }
}

int main()
{
  scenario0();
}