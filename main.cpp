#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <scoped_allocator>
#include <assert.h>
#include <string>
#include <memory_resource>

class custom_monotonic_buffer_resource : std::pmr::monotonic_buffer_resource
{
  memory_resource* _Resource = std::pmr::get_default_resource();
  void *do_allocate(const size_t _Bytes, const size_t _Align) override { return _Resource}
  void do_deallocate(void *, size_t, size_t) override {} // nothing to do
}

scenario0()
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