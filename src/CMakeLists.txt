set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x")
add_library (sgw_gc alloc.hpp chunk.cpp chunk.hpp)
