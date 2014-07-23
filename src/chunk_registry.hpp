#ifndef _CHUNK_REGISTRY_HPP
#define _CHUNK_REGISTRY_HPP

#include <cassert>
#include <stdlib.h>

#include "chunk.hpp"
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
  add (chunk *c);

  void
  remove (chunk *c);

  chunk*
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
