#include <cassert>
#include "alloc.hpp"

alloc::alloc () :
  list_objects_max_size (32)
{
}

alloc::~alloc ()
{
  for (auto chnk : small_chunks)
    {
      ch_alloc.free (chnk);
    }
}

void*
alloc::allocate (std::size_t size)
{
  if (size <= list_objects_max_size)
    {
      return allocate_from_list (size, freelists[size]);
    }
  else
    {
      return chunk_allocate (size)->data();
    }
}

void
alloc::free (void* ptr)
{
  chunk *chnk = ch_reg.find_chunk (ptr);
  assert (chnk);
  auto size = chnk->object_size;
  if (size <= list_objects_max_size)
    {
      freelists[size].push_front (ptr);
    }
  else
    {
      ch_alloc.free (chnk);
    }
}

void*
alloc::allocate_from_list (std::size_t size, freelist &list)
{
  if (list.empty()) 
    {
      add_to_freelist (size, list);
    }

  auto result = list.front();
  list.pop_front();
  return result;
}

void
alloc::add_to_freelist (std::size_t size, freelist &list)
{
  auto chnk = chunk_allocate (size);
  auto base = (char*) chnk->data();

  for (auto p = base; p < chnk->end(); p += size)
    {
      list.push_front (p);
    }

  small_chunks.push_back (chnk);
}

chunk*
alloc::chunk_allocate (std::size_t size)
{
  auto chnk = ch_alloc.allocate (size);

  if (chnk)
    {
      ch_reg.add (chnk);
    }

  return chnk;
}
