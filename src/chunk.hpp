#ifndef _CHUNK_HPP
#define _CHUNK_HPP

#include <cassert>
#include <memory>
#include <new>
#include <stdlib.h>

#include "bitmap.hpp"

#define CHUNK_BITS 12UL
#define CHUNK_SIZE (1 << CHUNK_BITS)

struct chunk
{
  static
  chunk*
  create (std::size_t object_size)
  {
    std::size_t rounded_size = chunk::rounded_size (object_size);

    void *ptr;
    posix_memalign (&ptr, CHUNK_SIZE, rounded_size);

    std::size_t num_objects = rounded_size / object_size;
    assert (object_size >= CHUNK_SIZE/2
	    ? num_objects == 1 
	    : num_objects > 1);

    chunk *c = new (ptr) chunk (object_size, num_objects);

    return c;
  }    

  static
  std::size_t
  rounded_size(std::size_t object_size)
  {
    std::size_t num_chunks = (object_size / CHUNK_SIZE) + 1;
    std::size_t rounded_size = num_chunks * CHUNK_SIZE;

    std::size_t remaining_space = rounded_size - sizeof(chunk);
    if (remaining_space < object_size)
      {
	rounded_size += CHUNK_SIZE;
      }

    return rounded_size;
  }

  static
  void
  destroy (chunk* chunk)
  {
    chunk->~chunk();
    free (chunk);
  }

  chunk (std::size_t object_size, std::size_t num_objects) :
    object_size (object_size),
    mark_bitmap (num_objects)
  {
  }

  bool
  can_split()
  {
    return size() > CHUNK_SIZE;
  }


  bool
  can_split_into (std::size_t new_size)
  {
    return can_split () && rounded_size (new_size) < size ();
  }


  chunk*
  split (std::size_t new_size)
  {
    assert (can_split_into (new_size));

    auto new_other_size = rounded_size(new_size);
    auto new_this_size  = size() - new_other_size;
    object_size         = new_this_size - sizeof(chunk);

    assert ((char*)this + new_this_size == end());

    chunk *c = new (end()) chunk (new_size, new_other_size / new_size);

    return c;
  }

  bool
  can_merge (chunk *other)
  {
    return (void*) other == end();
  }

  void
  merge (chunk *other)
  {
    assert (can_merge (other));

    object_size = other->size() + effective_size();
  }

  std::size_t
  size()
  {
    return rounded_size (object_size);
  }

  std::size_t
  effective_size()
  {
    return size () - sizeof(chunk);
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

  void*
  end()
  {
    return ((char*)this) + rounded_size(object_size);
  }

  /*
    Does the argument point to the start of an object within this chunk.
   */
  bool
  valid (void *ptr)
  {
    std::size_t offset = (char*)ptr - (char*)data();
    bool in_chunk_bounds = offset < CHUNK_SIZE - sizeof(chunk);
    return in_chunk_bounds && offset % object_size == 0;
  }

  std::size_t object_size;
  bitmap mark_bitmap;
  std::shared_ptr<chunk> *back_ptr;
};

#endif
