#ifndef _CHUNK_ALLOCATOR_HPP
#define _CHUNK_ALLOCATOR_HPP

#include <set>

#include "chunk.hpp"

#define FIRST_CHUNK_SIZE (1 << 14)

struct chunk_allocator
{
  chunk_allocator()
  {
    total_allocation = FIRST_CHUNK_SIZE;
    chunk *chnk = chunk::create (total_allocation);
    freeset.insert (chnk);
  }

  ~chunk_allocator()
  {
    for (auto chnk : freeset)
      {
	chunk::destroy (chnk);
      }
  }

  chunk*
  allocate (std::size_t size)
  {
    auto end = freeset.end();

    for (auto it = freeset.begin(); it != end; ++it)
      {
	auto chnk = *it;
	if (chnk->can_split_into (size))
	  {
	    return chnk->split (size);
	  }
	else if (size < chnk->effective_size())
	  {
	    chnk = new (chnk) chunk (size);
	    freeset.erase (it);
	    return chnk;
	  }
      }

    return nullptr;
  }

  void
  free(chunk *chnk)
  {
    assert (chnk);
    auto it = freeset.upper_bound (chnk);
    auto greater_it = it;
    auto merged_chunk = false;

    if (greater_it != freeset.end())
      {
	// Merge great element into argument
	auto greater = *greater_it;

	if (chnk->can_merge (greater))
	  {
	    chnk->merge (greater);
	    freeset.erase (greater_it);
	  }
      }

    if (!freeset.empty())
      {
	// Check the element less than one greater to see if
	// we can merge into it.
	auto lesser = *--it;
	if (lesser->can_merge (chnk))
	  {
	    merged_chunk = true;
	    lesser->merge (chnk);
	  }
      }

    if (!merged_chunk)
      {
	freeset.insert (chnk);
      }
  }

  std::size_t total_allocation;
  std::set <chunk*> freeset;
};

#endif
