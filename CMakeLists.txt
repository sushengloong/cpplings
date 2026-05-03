cmake_minimum_required(VERSION 3.16)

project(cpplings LANGUAGES CXX)

add_executable(cpplings src/main.cpp)
target_compile_features(cpplings PRIVATE cxx_std_17)

if(MSVC)
  target_compile_options(cpplings PRIVATE /W4)
else()
  target_compile_options(cpplings PRIVATE -Wall -Wextra -pedantic)
endif()
