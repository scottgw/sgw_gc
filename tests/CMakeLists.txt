set (test_names
  alloc_test
  chunk_allocator_test
  chunk_registry_test
  chunk_test
  bitmap_test
  pointer_table_test
  unwind_test
  CACHE INTERNAL ""
  )
# The `CACHE INTERNAL' line  makes this variable available in the parent,
# which is useful for running the tests from there.

if (UNIX)
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x")
endif (UNIX)

if (WIN32)
   include_directories ("C:\\Users\\scott\\gtest-1.7.0\\include")
endif (WIN32)

find_library (GTEST_LIBRARY NAMES gtest
	      PATHS "C:\\Users\\scott\\gtest-1.7.0\\build\\Debug")
find_library (GTEST_MAIN_LIBRARY NAMES gtest_main
	      PATHS "C:\\Users\\scott\\gtest-1.7.0\\build\\Debug")

find_package (Threads)
include_directories (${sgw_gc_SOURCE_DIR})

foreach (test_name ${test_names})
  add_executable (${test_name} ${test_name}.cpp)
  target_link_libraries (${test_name}
    sgw_gc
    ${CMAKE_THREAD_LIBS_INIT}
    ${GTEST_LIBRARY}
    ${GTEST_MAIN_LIBRARY}
    )
endforeach (test_name)
