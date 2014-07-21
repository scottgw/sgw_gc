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
      return allocate_from_list (freelists [size]);
    }

  assert (false && "Only small allocations implemented");
}

void*
alloc::allocate_from_list (freelist &list)
{
  if (list.empty()) 
    {
      // allocate new page
    }
  else
    {
      auto result = list.front();
      list.pop_front();
      return result;
    }
}
