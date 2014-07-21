cmake_minimum_required(VERSION 2.8)

project (SGW_GC)
add_subdirectory (src)


add_subdirectory (tests)
enable_testing ()
add_test (sgw_gc_test tests/sgw_gc_test)