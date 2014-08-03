#include "chunk.hpp"

chunk*
chunk::create (std::size_t object_size)
{
  std::size_t rounded_size = chunk::rounded_size (object_size);

  void *ptr;
  auto res = posix_memalign (&ptr, CHUNK_SIZE, rounded_size);
  assert (res == 0);

  chunk *c = new (ptr) chunk (object_size);

  return c;
}    

std::size_t
chunk::rounded_size(std::size_t object_size)
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


void
chunk::destroy (chunk* chunk)
{
  chunk->~chunk();
  free (chunk);
}


chunk*
chunk::split (std::size_t new_size)
{
  assert (can_split_into (new_size));

  auto new_other_size = rounded_size(new_size);
  auto new_this_size  = size() - new_other_size;
  object_size         = new_this_size - sizeof(chunk);
  // If this is splittable then it should already have a bitmap size of 1  
  // mark_bitmap         = bitmap (1);
  assert (mark_bitmap.size == 1);

  assert ((char*)this + new_this_size == end());

  chunk *c = new (end()) chunk (new_size);

  return c;
}

void
chunk::set_mark (void* ptr)
{
  mark_bitmap.set (ptr_to_object_idx(ptr), true);
}


void
chunk::clear_mark (void* ptr)
{
  mark_bitmap.set (ptr_to_object_idx(ptr), false);
}

bool
chunk::is_marked (void* ptr)
{
  return mark_bitmap [ptr_to_object_idx(ptr)];
}



bool
chunk::valid (void *ptr)
{
  std::size_t offset = (char*)ptr - (char*)data();
  bool in_chunk_bounds =
    (char*) ptr >= (char*) data() &&
    offset < effective_size();
  bool valid_offset = offset % object_size == 0;
  return in_chunk_bounds && valid_offset;
}
