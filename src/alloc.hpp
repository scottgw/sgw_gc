#include <atomic>
#include <condition_variable>
#include <deque>
#include <map>
#include <mutex>
#include <unordered_set>
#include <stdlib.h>

#include "chunk_registry.hpp"
#include "chunk_allocator.hpp"

struct freelist : public std::deque<void*>
{
};

class alloc {
public:
  alloc (bool free_memory = true);

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
      total_threads (0),
      remaining_threads (0),
      finished_gc (true)
    {
    }

    bool
    join()
    {
      std::unique_lock<std::mutex> lock (mutex);
      finished_gc = false;
      remaining_threads--;
      if (remaining_threads > 0)
	{
	  cv.wait (lock, [&](){ return finished_gc; });
	  remaining_threads++;
	  return false;
	}
      else
	{
	  return true;
	}
    }

    void
    finish()
    {
      std::unique_lock<std::mutex> lock (mutex);
      cv.notify_all();
      finished_gc = true;
    }

    void add_new_thread ()
    {
      std::unique_lock<std::mutex> lock (mutex);
      total_threads++;
      cv.wait (lock, [&](){ return finished_gc; });
      remaining_threads++;
    }

    std::atomic<int> total_threads;
    std::atomic<int> remaining_threads;
    bool finished_gc;

  private:
    std::mutex mutex;
    std::condition_variable cv;
  };

public:
  barrier gc_barrier;

private:
  bool free_memory;
};
