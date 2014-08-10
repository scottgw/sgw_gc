if (APPLE)
   find_library (SYSTEM_LIBRARY System)
   set (EXTRA_LIBS ${SYSTEM_LIBRARY})
elseif (UNIX)
   set (EXTRA_LIBS unwind)
elseif (WIN32)
   find_library (EXTRA_LIBS DbgHelp)
endif (APPLE)

if (UNIX)
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x")
endif (UNIX)

add_library (sgw_gc
  alloc.cpp alloc.hpp
  pointer_table.hpp
  chunk_allocator.hpp
  chunk_registry.cpp chunk_registry.hpp
  chunk.cpp chunk.hpp
  bitmap.hpp
  unwind.hpp
  )

target_link_libraries (sgw_gc
  ${EXTRA_LIBS}
  )
