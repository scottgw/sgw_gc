#ifndef _CHUNK_REGISTRY_HPP
#define _CHUNK_REGISTRY_HPP

#include <cassert>
#include <stdlib.h>

#include "chunk_header.hpp"
#include "pointer_table.hpp"

struct chunk_registry
{
  chunk_registry ();

  bool
  in_range (void *ptr)
  {
    return ptr >= m_lower && ptr <= m_upper;
  }

  void
  register_header (chunk_header *header);

  chunk_header*
  new_chunk_header (std::size_t object_size);

  void*
  new_chunk (std::size_t object_size)
  {
    return new_chunk_header(object_size)->data();
  }

  chunk_header*
  find_chunk (void *ptr);

  void
  mark(void *ptr);

  bool
  is_marked(void *ptr);

private:
  pointer_table ptr_table;

private:
  void *m_lower;
  void *m_upper;

};

#endif
