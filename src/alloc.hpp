#include <forward_list>
#include <map>
#include <vector>
#include <stdlib.h>

#include "chunk_registry.hpp"
#include "chunk_allocator.hpp"

struct freelist : public std::forward_list<void*>
{
};

class alloc {
public:
  alloc ();

  ~alloc ();

  void*
  allocate (std::size_t size);

  void
  free (void* ptr);

private:
  void*
  allocate_from_list (std::size_t size, freelist &list);

  std::map<std::size_t, freelist> freelists;

private:
  const std::size_t list_objects_max_size;

private:
  void
  add_to_freelist (std::size_t size, freelist &list);

  chunk*
  chunk_allocate (std::size_t size);

  chunk_registry ch_reg;
  chunk_allocator ch_alloc;

  std::vector <chunk*> small_chunks;
};
