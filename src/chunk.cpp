#include <cassert>
#include <stdlib.h>
#include "chunk.hpp"

chunk_allocator::chunk_allocator ():
  m_lower ((void*)(1UL << 63UL)),
  m_upper (0)
{

}

void*
chunk_allocator::new_chunk (std::size_t object_size)
{
  auto header = std::shared_ptr<chunk_header> (chunk_header::create (object_size));
  auto ptr = header->data;

  m_lower = std::min(m_lower, ptr);
  m_upper = std::max(m_upper, ptr);

  std::shared_ptr<level2_table> table2;
  auto it = m_table1.find (TOP_BITS(ptr));
  if (it == m_table1.end())
    {
      table2 = std::make_shared<level2_table>();
      m_table1 [TOP_BITS(ptr)] = table2;
    }
  else
    {
      table2 = it->second;
    }

  auto &table_header = (*table2) [MID_BITS(ptr)];
  table_header = header;
  header->back_ptr = &table_header;

  return ptr;
}

chunk_header*
chunk_allocator::find_chunk (void* ptr)
{
  auto it = m_table1.find (TOP_BITS(ptr));

  if (it == m_table1.end())
    {
      return nullptr;
    }

  auto table2 = it->second;
  auto entry = (*table2)[MID_BITS(ptr)];

  if (!entry)
    {
      return nullptr;
    }

  return entry.get();
}

void
chunk_allocator::mark (void* ptr)
{
  auto header = find_chunk (ptr);
  assert (header && "Must be a valid ptr");
  header->mark (ptr);
}

bool
chunk_allocator::is_marked (void* ptr)
{
  auto header = find_chunk (ptr);
  assert (header && "Must be a valid ptr");
  return header->is_marked (ptr);
}
