#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <assert.h>
#include <string>
#include <memory_resource>

// class custom_monotonic_buffer_resource : std::pmr::monotonic_buffer_resource
// {
//   memory_resource *_Resource = std::pmr::get_default_resource();
//   void *do_allocate(const size_t _Bytes, const size_t _Align) override { return _Resource }
//   void do_deallocate(void *, size_t, size_t) override {} // nothing to do
// };

class custom_resource : public std::pmr::memory_resource
{
public:
  explicit custom_resource(std::pmr::memory_resource *up = std::pmr::get_default_resource())
      : _upstream{up}
  {
  }

  void *do_allocate(size_t bytes, size_t alignment) override
  {
    return _upstream;
  }

  void do_deallocate(void *ptr, size_t bytes, size_t alignment) override
  {
  }

  bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override
  {
    return this == &other;
  }

private:
  std::pmr::memory_resource *_upstream;
};

void scenario0()
{
  std::byte buffer[128];
  custom_resource resource;
  std::pmr::monotonic_buffer_resource pool{std::data(buffer), std::size(buffer), &resource};
  std::pmr::vector<int> v(&pool);
  assert(v.size() == 0);
  for (int i = 0; i < 50; i++)
  {
    v.push_back(i);
  }

  // std::cout << sizeof(int) * sizeof(buffer) << std::endl;
  // std::cout << sizeof(int) * v.capacity() << std::endl;

  assert(v.size() == 50);

  for (int i = 0; i < 10; i++)
  {
    try
    {
      v.push_back(i);
      // std::cout << sizeof(int) * v.capacity() << " " << i << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cout << "Caught an expected buffer overflow error - at index i: " << i << std::endl;
    }
  }
}

void scenario1()
{
  std::byte buffer[512]; // too large i think
  custom_resource resource;
  std::pmr::monotonic_buffer_resource pool{std::data(buffer), std::size(buffer), &resource};
  std::pmr::vector<std::pmr::vector<int>> outer(&pool);

  int rows = 5, columns = 5;
  for (int i = 0; i < rows; i++)
  {
    outer.push_back(std::pmr::vector<int>(&pool));
  }
  assert(outer.size() == rows);

  int expected_sum = 0;
  for (int i = 0; i < outer.size(); i++)
  {
    for (int j = 0; j < columns; j++)
    {
      outer[i].push_back(j);
      expected_sum += j;
    }
  }

  for (int i = 0; i < outer.size(); i++)
  {
    assert(outer[i].size() == columns);
  }

  int actual_sum = 0;
  for (int i = 0; i < outer.size(); i++)
  {
    for (int j = 0; j < outer[i].size(); j++)
    {
      actual_sum += outer[i][j];
    }
  }

  assert(actual_sum == expected_sum);
}

void scenario2()
{
    auto scope = []() {
      std::byte buffer[128];
      custom_resource resource;
      std::pmr::monotonic_buffer_resource pool{std::data(buffer), std::size(buffer), &resource};
      std::pmr::vector<int> v(&pool);
      return v;
    }; 

    auto v = scope();
    v.push_back(1); // SHOULD FAIL OUT OF SCOPE - stack memory of buffer is only accessible in its scope
}

int main()
{
  // scenario0();
  // scenario1();
  // scenario2();
}