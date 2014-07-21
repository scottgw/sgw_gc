#include <cassert>
#include <memory>
#include <new>
#include <stdlib.h>

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

  static
  void
  destroy (chunk_header* header)
  {
    header->~chunk_header();
    free (header);
  }

  chunk_header (std::size_t object_size, std::size_t num_objects) :
    object_size (object_size),
    mark_bitmap (num_objects)
  {
  }

  void
  mark (void* ptr)
  {
    int i = ((char*) ptr - (char*) data()) / object_size;
    mark_bitmap.set (i, true);
  }


  bool
  is_marked (void* ptr)
  {
    int i = ((char*) ptr - (char*) data()) / object_size;
    return mark_bitmap [i];
  }


  void
  clear_marks()
  {
    mark_bitmap.clear();
  }

  void*
  data()
  {
    return this + sizeof(*this);
  }

  /*
    Does the argument point to the start of an object within this chunk.
   */
  bool
  valid (void *ptr)
  {
    std::size_t offset = (char*)ptr - (char*)data();
    bool in_chunk_bounds = offset < CHUNK_SIZE - sizeof(chunk_header);
    return in_chunk_bounds && offset % object_size == 0;
  }

  std::size_t object_size;
  bitmap mark_bitmap;
  std::shared_ptr<chunk_header> *back_ptr;
};
