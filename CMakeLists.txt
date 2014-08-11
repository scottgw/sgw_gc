cmake_minimum_required(VERSION 2.8)
project (SGW_GC)

if (CMAKE_SIZEOF_VOID_P MATCHES "8")
   add_definitions ( -DPOINTER_64BIT )
else (CMAKE_SIZEOF_VOID_P MATCHES "8")
   add_definitions ( -DPOINTER_32BIT )
endif (CMAKE_SIZEOF_VOID_P MATCHES "8")

add_subdirectory (src)

add_subdirectory (tests)
enable_testing ()
foreach (test_name ${test_names})
  add_test (${test_name} ../gdb_test.sh tests/${test_name})
endforeach (test_name)
