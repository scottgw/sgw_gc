#include <cassert>
#include "alloc.hpp"

alloc::alloc () :
  list_objects_max_size (32)
{
}

void*
alloc::allocate (std::size_t size)
{
  if (size <= list_objects_max_size)
    {
      return allocate_from_list (size, freelists[size]);
    }

  assert (false && "Only small allocations implemented");
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
  auto header = ch_alloc.new_chunk_header (size);
  auto base = (char*) header->data();

  for (auto p = base; p < base + chunk_header::rounded_size(size); p += size)
    {
      list.push_front (p);
    }
}
