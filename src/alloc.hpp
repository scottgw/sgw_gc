#include <forward_list>
#include <map>
#include <cassert>
#include <vector>
#include <stdlib.h>

#include "chunk.hpp"

struct freelist : public std::forward_list<void*>
{
};

class alloc {
public:
  alloc ();

  void*
  allocate (std::size_t size);

  void*
  allocate_from_list (freelist &list);

private:
  std::map<std::size_t, freelist> freelists;

private:
  const std::size_t list_objects_max_size;

private:
  chunk_allocator ch_alloc;

};
