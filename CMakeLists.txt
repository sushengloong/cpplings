cmake_minimum_required(VERSION 3.16)

project(cpplings LANGUAGES CXX)

set(CPPLINGS_LIB_SOURCES
  src/cli.cpp
  src/manifest.cpp
  src/output.cpp
  src/progress.cpp
  src/runner.cpp
  src/util.cpp
  src/worktree.cpp
)

add_library(cpplings_lib STATIC ${CPPLINGS_LIB_SOURCES})
target_include_directories(cpplings_lib PUBLIC include)
target_compile_features(cpplings_lib PUBLIC cxx_std_17)

add_executable(cpplings src/main.cpp)
target_link_libraries(cpplings PRIVATE cpplings_lib)

set(CPPLINGS_TESTS
  manifest
  worktree
  progress
  output
)

foreach(test_name IN LISTS CPPLINGS_TESTS)
  add_executable(test_${test_name} tests/test_${test_name}.cpp)
  target_link_libraries(test_${test_name} PRIVATE cpplings_lib)
endforeach()

if(MSVC)
  target_compile_options(cpplings_lib PRIVATE /W4)
  target_compile_options(cpplings PRIVATE /W4)
  foreach(test_name IN LISTS CPPLINGS_TESTS)
    target_compile_options(test_${test_name} PRIVATE /W4)
  endforeach()
else()
  target_compile_options(cpplings_lib PRIVATE -Wall -Wextra -pedantic)
  target_compile_options(cpplings PRIVATE -Wall -Wextra -pedantic)
  foreach(test_name IN LISTS CPPLINGS_TESTS)
    target_compile_options(test_${test_name} PRIVATE -Wall -Wextra -pedantic)
  endforeach()
endif()

enable_testing()
foreach(test_name IN LISTS CPPLINGS_TESTS)
  add_test(NAME test_${test_name} COMMAND test_${test_name})
endforeach()
