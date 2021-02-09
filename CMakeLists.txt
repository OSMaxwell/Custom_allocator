cmake_minimum_required(VERSION 3.13)

#set the project name
project(Custom_allocator VERSION 1.0)

#Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#add the executable
add_executable(Custom_allocator main.cpp)

target_link_libraries(Custom_allocator PUBLIC
                        "${")   #CONTINUE https://cmake.org/cmake/help/latest/guide/tutorial/index.html#id3


