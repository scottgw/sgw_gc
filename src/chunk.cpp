#include <cassert>
#include "chunk.hpp"

chunk_allocator::chunk_allocator ():
  m_lower (0),
  m_upper ((void*)(1UL << 63UL))
{

}

void*
chunk_allocator::new_chunk (std::size_t object_size)
{
  std::size_t num_chunks = (object_size / CHUNK_SIZE) + 1;
  std::size_t rounded_size = num_chunks * CHUNK_SIZE;
  void *ptr = aligned_alloc (CHUNK_SIZE, rounded_size);
  void *upper = (char*)ptr + rounded_size;

  m_lower = std::min(m_lower, ptr);
  m_upper = std::max(m_upper, upper);

  auto header = std::make_shared<chunk_header>(rounded_size / object_size);
  header->data = ptr;
  header->object_size = object_size;
  header->mark_bitmap.clear ();

  auto it = m_table1.find (TOP_BITS(ptr));
  if (it == m_table1.end())
    {
      auto table2 = std::make_shared<level2_table>();
      (*table2) [MID_BITS(ptr)] = header;
      m_table1 [TOP_BITS(ptr)] = table2;
    }
  else
    {
      (*it->second)[MID_BITS(ptr)] = header;
    }

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
