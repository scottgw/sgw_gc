#ifndef _CHUNK_HPP
#define _CHUNK_HPP

#include <cassert>
#include <memory>
#include <new>
#include <stdlib.h>

#include "bitmap.hpp"

#ifdef POINTER_64BIT
#define PTR_TOP_BITS 40UL
#define CHUNK_BITS 12UL
#define PTR_MID_BITS (64UL - PTR_TOP_BITS - CHUNK_BITS)
#elif defined (POINTER_32BIT)
#define PTR_TOP_BITS 12UL
#define CHUNK_BITS 12UL
#define PTR_MID_BITS (32UL - PTR_TOP_BITS - CHUNK_BITS)
#endif

#define PTR_TOP_SIZE (1UL << PTR_TOP_BITS)
#define TOP_BIT_MASK (~(PTR_TOP_BITS - 1UL))
#define TOP_BITS(x) (((std::size_t) x) >> (PTR_MID_BITS + CHUNK_BITS))

#define PTR_MID_SIZE (1 << PTR_MID_BITS)
#define MID_BITS(x)							\
  ((((std::size_t) x) << PTR_TOP_BITS) >> (PTR_TOP_BITS + CHUNK_BITS))

#define CHUNK_SIZE (1 << CHUNK_BITS)

struct chunk
{
  static
  chunk*
  create (std::size_t object_size);

  static
  std::size_t
  rounded_size(std::size_t object_size);

  static
  void
  destroy (chunk* chunk);

  chunk (std::size_t object_size) :
    object_size (object_size),
    mark_bitmap (number_objects())
  {
  }

  std::size_t
  number_objects()
  {
    return effective_size() / object_size;
  }

  /*
    A chunk can be split if after the split both the new chunk
    and the old chunk are non-empty.
   */
  bool
  can_split_into (std::size_t new_size)
  {
    return
      size() > CHUNK_SIZE &&
      rounded_size (new_size) <= size () - CHUNK_SIZE;
  }

  chunk*
  split (std::size_t new_size);

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
    mark_bitmap = bitmap (1);
    other->~chunk();
  }

  /*
    Total size of the chunk (including header)
   */
  std::size_t
  size()
  {
    return rounded_size (object_size);
  }

  /*
    Size of the chunk available for program allocation.
   */
  std::size_t
  effective_size()
  {
    return size () - sizeof(chunk);
  }

  void
  set_mark (void* ptr);

  void
  clear_mark (void* ptr);

  bool
  is_marked (void* ptr);

  bool
  all_clear()
  {
    return mark_bitmap.all_clear();
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
    return ((char*)this) + size();
  }

  /*
    Does the argument point to the start of an object within this chunk.
   */
  bool
  valid (void *ptr);

  std::size_t object_size;
  bitmap mark_bitmap;
  chunk **back_ptr;

private:
  int
  ptr_to_object_idx(void *ptr)
  {
    return ((char*)ptr - (char*)data()) / object_size;
  }
};

#endif
