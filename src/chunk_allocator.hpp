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

  chunk*
  allocate (std::size_t size)
  {
    auto end = freeset.end();

    for (auto it = freeset.begin(); it != end; ++it)
      {
	auto chunk = *it;
	if (chunk->can_split_into (size))
	  {
	    return chunk->split (size);
	  }
	else if (size < chunk->effective_size())
	  {
	    freeset.erase (it);
	    return chunk;
	  }
      }

    return 0;
  }

  void
  free(chunk *chnk)
  {
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

private:
  std::size_t total_allocation;
  std::set <chunk*> freeset;
};

#endif
