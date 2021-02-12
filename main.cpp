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
  const int STRING_BUFFER_MAX_SIZE = 100;
  // pay attention to this line
  using string = std::basic_string<char, std::char_traits<char>, CustomStringAllocator<char>>;
  string *buffer = (string *)malloc(STRING_BUFFER_MAX_SIZE * sizeof(string));
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

  try
  {
    // Should fail
    string outofbuffer_str("OSMAXWELLStarkaf", CustomStringAllocator<string>(&buffer[STRING_BUFFER_MAX_SIZE - 2], 25));
  }
  catch (const std::exception &e)
  {
    std::cout << "Caught an expected buffer overflow error";
  }

  free(buffer);
}

void scenario4()
{
  const int STRING_BUFFER_MAX_SIZE = 1000;
  // pay attention to this line
  // using string = std::basic_string<char, std::char_traits<char>, CustomStringAllocator<char>>;
  // string *buffer_string = (string *)malloc(STRING_BUFFER_MAX_SIZE * sizeof(string));
  // std::vector<string, StaticBufferAllocator<string>> user_names(0, StaticBufferAllocator<string>(&buffer_string[0], STRING_BUFFER_MAX_SIZE));

  // user_names.push_back(string("Dainerx", CustomStringAllocator<string>(&buffer_string[0], 10)));
  // user_names.push_back(string("OSMAXWELL", CustomStringAllocator<string>(&buffer_string[10], 12)));
  // user_names.push_back(string("Starkaf", CustomStringAllocator<string>(&buffer_string[30], 10)));
  // assert(user_names.size() == 3);
  // for (std::size_t i = 0; i < user_names.size(); i++)
  // {
  //   assert(user_names[i].size() >= 7); //shortest is 7 long
  // }
}

int main()
{
  // scenario1();
  // scenario2();
  scenario3(); // scenario 3 prints three custom allocated strings
}