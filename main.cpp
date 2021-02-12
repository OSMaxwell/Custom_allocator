#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <assert.h>
#include <string>
#include "StaticBufferAllocator.h"
#include "CustomStringAllocator.h"

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

// let's play with strings before going into strings inside vectors
// remember string are containers and they grow with the default allocator,
// hence the CustomStringAllocator.
void scenario3()
{
  // pay attention to this line
  using string = std::basic_string<char, std::char_traits<char>, CustomStringAllocator<char>>;
  string *buffer = (string *)malloc(100 * sizeof(string));
  string s1("OSMAXWELL", CustomStringAllocator<string>(&buffer[0], 20));
  string s2("Starkaf", CustomStringAllocator<string>(&buffer[25], 25));
  // let's try some operators
  assert(s1 == "OSMAXWELL");
  assert(s2 == "Starkaf");
  // let's try some operations inside vectors
  std::vector<string> user_names = {string("Dainerx", CustomStringAllocator<string>(&buffer[60], 30))};
  assert(user_names.size() == 1);
  user_names.push_back(s1);
  user_names.push_back(s2);
  assert(user_names.size() == 3);

  for (std::size_t i = 0; i < user_names.size(); i++)
  {
    std::cout << user_names[i] << std::endl;
  }

  free(buffer);
}

void scenario4()
{
}

int main()
{
  // scenario1();
  // scenario2();
  scenario3(); // scenario 3 prints three custom allocated strings
}