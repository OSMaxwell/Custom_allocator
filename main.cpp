#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <string>
#include "StaticBufferAllocator.h"

void scenario1()
{
  // vector with custom allocator class.
  int buffer[100] = {0}; // this buffer hold 100 integers.
  std::vector<int, StaticBufferAllocator<int>> v(0, StaticBufferAllocator<int>(&buffer[0], 100));
  std::vector<int, StaticBufferAllocator<int>> v1(0, StaticBufferAllocator<int>(&buffer[49], 100));
  assert(v.size() == 0);
  assert(v1.size() == 0);

  for (int i = 0; i < 10; i++)
  {
    v.push_back(i);
    v1.push_back(-i);
  }

  assert(v.size() == 10);
  assert(v1.size() == 10);

  for (int i = 0; i < 10; i++)
  {
    assert(v[i] == i);
    assert(v1[i] == -i);
  }
}

void scenario2()
{
  // pay attention to this line
  using stringBuffer = std::basic_string<char, std::char_traits<char>, StaticBufferAllocator<char>>;
  char *buffer = (char *)malloc(100 * sizeof(char));
  std::vector<stringBuffer, StaticBufferAllocator<stringBuffer>> v(0, StaticBufferAllocator<stringBuffer>(&buffer[0], 100));
  std::vector<std::string> seed = {"OS", "MAXWELL", "Dainer", "Dainerx"};

  assert(v.size() == 0);
  assert(seed.size() == 4);

  v.resize(10);
  for (int i = 0; i < 10; i++)
  {
    //seed[i % seed.size()].copy((&v[i]).c_str(), seed[i % seed.size()].size(), 0);
  }

  std::cout << std::endl;
  std::cout << std::endl;

  assert(v.size() == 10);

  for (int i = 0; i < 10; i++)
  {
    std::cout << v[i] << std::endl;
    // printf("%s,%s - %d\n", v[i], std::to_string(i), v[i].compare(std::to_string(i)));
    // assert(v[i].compare(std::to_string(i)) == 0);
  }

  free(buffer);
}

void scenario3()
{
  struct Point
  {
    int x;
    int y;
    bool operator==(const Point &other)
    {
      return x == other.x && y == other.y;
    }
  };

  Point buffer[100]; // this buffer hold 100 Points.
  std::vector<Point, StaticBufferAllocator<Point>> v(0, StaticBufferAllocator<Point>(&buffer[0], 100));
  assert(v.size() == 0);

  for (int i = 0; i < 10; i++)
  {
    v.push_back(Point{i, -i});
  }

  assert(v.size() == 10);

  for (int i = 0; i < 10; i++)
  {
    auto p = Point{i, -i};
    assert(v[i] == p);
  }
}

int main()
{
  scenario1();
  //scenario2();
  scenario3();
  // vector 2 with custom allocator storing vectors
}