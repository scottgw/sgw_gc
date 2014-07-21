set (test_names
  alloc_test
  chunk_test
  bitmap_test
  CACHE INTERNAL ""
  )
# The `CACHE INTERNAL' line  makes this variable available in the parent,
# which is useful for running the tests from there.

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
find_package (Threads)
include_directories (${sgw_gc_SOURCE_DIR})

foreach (test_name ${test_names})
  add_executable (${test_name} ${test_name}.cpp)
  target_link_libraries (${test_name}
    sgw_gc
    ${CMAKE_THREAD_LIBS_INIT}
    gtest
    gtest_main)
endforeach (test_name)
