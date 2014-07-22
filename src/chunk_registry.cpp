#include <cassert>
#include <stdlib.h>
#include "chunk_registry.hpp"

chunk_registry::chunk_registry ():
  m_lower ((void*)(1UL << 63UL)),
  m_upper (0)
{

}

void
chunk_registry::register_header (chunk_header *header)
{
  auto shared_header =
    std::shared_ptr<chunk_header> (header, chunk_header::destroy);
  auto ptr = header->data();
  auto &header_ref = ptr_table [ptr];

  header_ref = shared_header;
  header->back_ptr = &header_ref;

  m_lower = std::min(m_lower, ptr);
  m_upper = std::max(m_upper, ptr);
}

chunk_header*
chunk_registry::new_chunk_header (std::size_t object_size)
{
  auto header = chunk_header::create (object_size);
  register_header (header);

  return header;
}

chunk_header*
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
