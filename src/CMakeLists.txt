set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x")
add_library (sgw_gc
  alloc.cpp alloc.hpp
  pointer_table.hpp
  chunk_registry.cpp chunk_registry.hpp
  chunk.cpp chunk.hpp
  bitmap.hpp
  )
