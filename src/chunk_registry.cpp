#include <cassert>
#include <stdlib.h>
#include "chunk_registry.hpp"

chunk_registry::chunk_registry ():
  m_lower ((void*)(1UL << 63UL)),
  m_upper (0)
{

}

void
chunk_registry::add (chunk *chnk)
{
  auto shared_chunk =
    std::shared_ptr<chunk> (chnk, chunk::destroy);
  auto ptr = chnk->data();
  auto &chunk_ref = ptr_table [ptr];

  chunk_ref = shared_chunk;
  chnk->back_ptr = &chunk_ref;

  m_lower = std::min(m_lower, ptr);
  m_upper = std::max(m_upper, ptr);
}

chunk*
chunk_registry::new_chunk (std::size_t object_size)
{
  auto chnk = chunk::create (object_size);
  add (chnk);

  return chnk;
}

chunk*
chunk_registry::find_chunk (void* ptr)
{
  if (!ptr_table.has (ptr))
    {
      return nullptr;
    }

  return ptr_table [ptr].get();
}

void
chunk_registry::mark (void* ptr)
{
  auto header = find_chunk (ptr);
  assert (header && "Must be a valid ptr");
  header->mark (ptr);
}

bool
chunk_registry::is_marked (void* ptr)
{
  auto header = find_chunk (ptr);
  assert (header && "Must be a valid ptr");
  return header->is_marked (ptr);
}
