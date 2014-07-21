#include <forward_list>
#include <map>
#include <cassert>
#include <vector>

#include <stdlib.h>


struct freelist : public std::forward_list<void*>
{
};

class alloc {
public:
  alloc () :
    list_objects_max_size (32)
  {
  }

  void*
  allocate (std::size_t size)
  {
    if (size <= list_objects_max_size)
      {
	return allocate_from_list (freelists [size]);
      }

    assert (false && "Only small allocations implemented");
  }

  void*
  allocate_from_list (freelist &list)
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

private:
  std::map<std::size_t, freelist> freelists;

private:
  const std::size_t list_objects_max_size;

};
