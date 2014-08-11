#include <atomic>
#include <condition_variable>
#include <deque>
#include <map>
#include <mutex>
#include <unordered_set>
#include <stdlib.h>

#include "chunk_registry.hpp"
#include "chunk_allocator.hpp"

struct freelist : public std::vector<void*>
{
};

class alloc {
public:
  alloc (void** stack_bottom, bool free_memory = true);

  ~alloc ();

  void*
  allocate (std::size_t size);

  void
  free (void* ptr);

private:
  void
  mark(void *ptr);

  void
  mark_stack();

  void
  sweep();

  void
  collect();

private:
  void **stack_bottom;

private:
  void*
  allocate_from_list (std::size_t size, freelist &list);

  std::map<std::size_t, freelist> freelists;

private:
  const std::size_t list_objects_max_size;

private:
  void
  add_to_freelist (std::size_t size, freelist &list);

  chunk*
  chunk_allocate (std::size_t size);

  chunk_registry ch_reg;
  chunk_allocator ch_alloc;

  std::unordered_set <chunk*> allocated_chunks;

private:
  struct barrier
  {
    barrier():
      remaining_threads (0),
      gc_required (false)
    {
    }

    bool
    trigger()
    {
      std::unique_lock<std::mutex> lock (mutex);
      if (gc_required)
	{
	  join_inner(lock);
	  return false;
	}
      else
	{
	  gc_required = true;
	  remaining_threads--;
	  cv.wait (lock, [&]() { return remaining_threads == 0; });
	  return true;
	}
    }

    void
    join_if_required()
    {
      if (gc_required)
	{
	  std::unique_lock<std::mutex> lock (mutex);
	  join_inner (lock);
	}
    }

    void
    finish()
    {
      std::unique_lock<std::mutex> lock (mutex);
      cv.notify_all();
      remaining_threads++;
      gc_required = false;
    }

    void add_thread ()
    {
      std::unique_lock<std::mutex> lock (mutex);
      if (gc_required)
	{
	  cv.wait (lock, [&](){ return !gc_required; });
	}
      remaining_threads++;
    }

    void remove_thread ()
    {
      std::unique_lock<std::mutex> lock (mutex);
      remaining_threads--;
      cv.notify_all();
    }

    int remaining_threads;
    std::atomic<bool> gc_required;
  private:
    void
    join_inner (std::unique_lock <std::mutex> &lock)
    {
      remaining_threads--;
      cv.notify_all ();
      cv.wait (lock, [&](){ return !gc_required; });
      remaining_threads++;
    }

    std::mutex mutex;
    std::condition_variable cv;
  };

public:
  barrier gc_barrier;

private:
  bool free_memory;
};
