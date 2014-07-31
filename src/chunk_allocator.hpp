#ifndef _CHUNK_ALLOCATOR_HPP
#define _CHUNK_ALLOCATOR_HPP

#include <set>

#include "chunk.hpp"

#define FIRST_CHUNK_SIZE (1 << 14)

struct chunk_allocator
{
  chunk_allocator(bool free_memory = true) :
    free_memory (free_memory),
    number_of_allocations (1),
    total_allocation (FIRST_CHUNK_SIZE)
  {
    chunk *chnk = chunk::create (total_allocation);
    freeset.insert (chnk);
  }

  ~chunk_allocator()
  {
    if (free_memory)
      {
	assert (freeset.size() == number_of_allocations &&
		"All allocations should be returned before desctruction");
	for (auto chnk : freeset)
	  {
	    chunk::destroy (chnk);
	  }
      }
  }

  void
  grow ()
  {
    freeset.insert (chunk::create (total_allocation));
    total_allocation *= 2;
    number_of_allocations++;
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
    auto merged_greater = false;

    if (greater_it != freeset.end())
      {
	// Merge great element into argument
	auto greater = *greater_it;

	if (chnk->can_merge (greater))
	  {
	    chnk->merge (greater);
	    merged_greater = true;
	  }
      }

    if (!freeset.empty())
      {
	// Check the element less than one greater to see if
	// we can merge into it.
	--it;
	auto lesser = *it;
	if (lesser->can_merge (chnk))
	  {
	    merged_chunk = true;
	    lesser->merge (chnk);
	  }
      }

    if (merged_greater)
      {
	freeset.erase (greater_it);
      }

    if (!merged_chunk)
      {
	freeset.insert (chnk);
      }
  }

  bool free_memory;
  std::size_t number_of_allocations;
  std::size_t total_allocation;
  std::set <chunk*> freeset;
};

#endif
