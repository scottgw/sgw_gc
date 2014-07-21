#include <new>
#include <array>
#include <map>
#include <vector>
#include <memory>
#include <stdlib.h>
#include <cassert>

#include "chunk_header.hpp"

#define PTR_TOP_BITS 40UL
#define PTR_TOP_SIZE (1UL << PTR_TOP_BITS)
#define TOP_BIT_MASK (~(PTR_TOP_BITS - 1UL))
#define TOP_BITS(x) (((std::size_t) x) >> (PTR_MID_BITS + CHUNK_BITS))

#define PTR_MID_BITS (64UL - PTR_TOP_BITS - CHUNK_BITS)
#define PTR_MID_SIZE (1 << PTR_MID_BITS)
#define MID_BITS(x)							\
  ((((std::size_t) x) << PTR_TOP_BITS) >> (PTR_TOP_BITS + CHUNK_BITS))


struct level2_table : public std::vector<std::shared_ptr<chunk_header>>
{
  level2_table ():
    std::vector<std::shared_ptr<chunk_header>>(PTR_MID_SIZE,
					       std::shared_ptr<chunk_header>())
  {
  }
};

struct level1_table : public std::map<std::size_t, std::shared_ptr<level2_table>>
{
};

struct chunk_allocator
{
  chunk_allocator ();

  bool
  in_range (void *ptr)
  {
    return ptr >= m_lower && ptr <= m_upper;
  }

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
  level1_table m_table1;

private:
  void *m_lower;
  void *m_upper;

};
