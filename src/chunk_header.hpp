#include <new>
#include <stdlib.h>
#include <cassert>

#include "bitmap.hpp"

#define CHUNK_BITS 12UL
#define CHUNK_SIZE (1 << CHUNK_BITS)

struct chunk_header
{
  static
  chunk_header*
  create (std::size_t object_size)
  {
    std::size_t num_chunks = (object_size / CHUNK_SIZE) + 1;
    std::size_t rounded_size = num_chunks * CHUNK_SIZE;

    std::size_t remaining_space = rounded_size - sizeof(chunk_header);
    if (remaining_space < object_size)
      {
	rounded_size += CHUNK_SIZE;
      }

    void *ptr;
    posix_memalign (&ptr, CHUNK_SIZE, rounded_size);

    std::size_t num_objects = rounded_size / object_size;
    assert (object_size >= CHUNK_SIZE/2
	    ? num_objects == 1 
	    : num_objects > 1);

    chunk_header *header = new (ptr) chunk_header (object_size, num_objects);

    return header;
  }    
  
  chunk_header (std::size_t object_size, std::size_t num_objects) :
    object_size (object_size),
    mark_bitmap (num_objects)
  {
    data = this + sizeof(chunk_header);
  }

  void
  mark (void* ptr)
  {
    int i = ((char*) ptr - (char*) data) / object_size;
    mark_bitmap.set (i, true);
  }


  bool
  is_marked (void* ptr)
  {
    int i = ((char*) ptr - (char*) data) / object_size;
    return mark_bitmap [i];
  }


  void
  clear_marks()
  {
    mark_bitmap.clear();
  }

  std::size_t object_size;
  bitmap mark_bitmap;
  void *data;
};
