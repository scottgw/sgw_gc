#include <forward_list>
#include <map>
#include <vector>
#include <stdlib.h>

#include "chunk_registry.hpp"

struct freelist : public std::forward_list<void*>
{
};

class alloc {
public:
  alloc ();

  void*
  allocate (std::size_t size);

  void*
  allocate_from_list (std::size_t size, freelist &list);

private:
  std::map<std::size_t, freelist> freelists;

private:
  const std::size_t list_objects_max_size;

private:
  void
  add_to_freelist (std::size_t size, freelist &list);

  chunk_registry ch_alloc;

};
