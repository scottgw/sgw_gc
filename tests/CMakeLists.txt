set (test_EXECUTABLE sgw_gc_test)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
include_directories (${sgw_gc_SOURCE_DIR})
add_executable (${test_EXECUTABLE} chunk_test.cpp)

target_link_libraries (${test_EXECUTABLE} sgw_gc gtest gtest_main)