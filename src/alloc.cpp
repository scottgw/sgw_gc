#include <cassert>
#include "alloc.hpp"
#include "unwind.hpp"

alloc::alloc (void **stack_bottom, bool free_memory) :
  stack_bottom (stack_bottom),
  list_objects_max_size (128),
  ch_alloc (free_memory),
  free_memory (free_memory)
{
}

alloc::~alloc ()
{
  if (free_memory)
    {
      for (auto chnk : allocated_chunks)
	{
	  ch_alloc.free (chnk);
	}
    }
}

void*
alloc::allocate (std::size_t size)
{
  if (size <= list_objects_max_size)
    {
      return allocate_from_list (size, freelists[size]);
    }
  else
    {
      auto chnk = chunk_allocate (size);
      if (chnk)
	{
	  return chnk->data();
	}
      else
	{
	  collect();
	  chnk = chunk_allocate (size);
	  if (!chnk)
	    {
	      ch_alloc.grow();
	      chnk = chunk_allocate (size);
	    }

	  return chnk->data();
	}
    }
}

void
alloc::free (void* ptr)
{
  chunk *chnk = ch_reg.find_chunk (ptr);
  assert (chnk);
  auto size = chnk->object_size;
  if (size <= list_objects_max_size)
    {
      freelists[size].push_back (ptr);
    }
  else
    {
      ch_alloc.free (chnk);
    }
}

void*
alloc::allocate_from_list (std::size_t size, freelist &list)
{
  if (list.empty())
    {
      add_to_freelist (size, list);
    }

  void* result;

  if (!list.empty())
    {
      result = list.back();
      list.pop_back();
    }
  else
    {
      result = nullptr;
    }

  return result;
}

void
alloc::add_to_freelist (std::size_t size, freelist &list)
{
  auto chnk = chunk_allocate (size);

  if (chnk)
    {
      auto base = (char*) chnk->data();

      for (auto p = base; p < chnk->end(); p += size)
	{
	  list.push_back (p);
	}
    }
}

chunk*
alloc::chunk_allocate (std::size_t size)
{
  auto chnk = ch_alloc.allocate (size);

  if (chnk)
    {
      ch_reg.add (chnk);
      allocated_chunks.insert (chnk);
    }

  return chnk;
}

void
alloc::mark (void *ptr)
{
  if (ch_reg.in_range (ptr))
    {
      auto chnk = ch_reg.find_chunk (ptr);
      if (chnk && chnk->valid(ptr) && !chnk->is_marked (ptr))
	{
	  chnk->set_mark (ptr);
	  auto size = chnk->object_size;
	  for (auto it = (void**) ptr; it < (void*)((char*)ptr + size) ; ++it)
	    {
	      auto slot = *it;
	      mark (slot);
	    }
	}
    }
}

void
alloc::mark_stack()
{
  unwind_stack stack (stack_bottom);
  auto it = stack.begin();
  auto end = stack.end();

  for (; it != end
	 ; ++it)
    {
      mark (*it);
    }
}

void
alloc::sweep()
{
  std::vector<chunk*> to_erase;
  for (auto chnk : allocated_chunks)
    {
      // This policy means that freelist-sized objects
      // will have their chunks returned if they are all free.
      if (chnk->all_clear())
	{
	  to_erase.push_back (chnk);
	}
      else
	{
	  auto base = chnk->data();
	  auto object_size = chnk->object_size;

	  for (auto p = (void**)base;
	       (char*) p + object_size < chnk->end();
	       p += object_size)
	    {
	      if (!chnk->is_marked (p) && object_size <= list_objects_max_size)
		{
		  freelists[object_size].push_back (p);
		}

	      chnk->clear_mark (p);
	    }
	}
    }

  for (auto chnk : to_erase)
    {
      allocated_chunks.erase (chnk);
      ch_reg.remove (chnk);
      ch_alloc.free (chnk);
    }
}

void
alloc::collect()
{
  mark_stack();
  sweep();
}
