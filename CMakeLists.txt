cmake_minimum_required(VERSION 2.8)
project (SGW_GC)

add_subdirectory (src)

add_subdirectory (tests)
enable_testing ()
foreach (test_name ${test_names})
  add_test (${test_name} tests/${test_name})
endforeach (test_name)
