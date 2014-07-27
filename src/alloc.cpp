#include <cassert>
#include "alloc.hpp"
#include "unwind.hpp"

alloc::alloc () :
  list_objects_max_size (128)
{
}

alloc::~alloc ()
{
  for (auto chnk : allocated_chunks)
    {
      ch_alloc.free (chnk);
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
	  if (chnk)
	    {
	      return chnk->data();
	    }
	  else
	    {
	      return nullptr;
	    }
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
      freelists[size].push_front (ptr);
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
      result = list.front();
      list.pop_front();
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
	  list.push_front (p);
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
      allocated_chunks.push_back (chnk);
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
	  for (auto it = (void**) ptr; it < (void**)ptr + size ; ++it)
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
  unwind_stack stack;

  for (auto ptr : stack)
    {
      mark (ptr);
    }
}

void
alloc::sweep()
{
  for (auto chnk : allocated_chunks)
    {
      auto base = chnk->data();
      auto size = chnk->object_size;
      for (auto p = (void**)base; p < chnk->end(); p += size)
	{
	  if (chnk->is_marked (p))
	    {
	      chnk->clear_mark (p);
	      break;
	    }
	  else
	    {
	      if (size > list_objects_max_size)
		{
		  ch_alloc.free (chnk);
		}
	      else
		{
		  freelists[size].push_front (p);
		}
	    }
	}      
    }
}

void
alloc::collect()
{
  mark_stack();
  sweep();
}
