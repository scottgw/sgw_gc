#include <future>
#include <thread>
#include <gtest/gtest.h>
#include "../src/alloc.hpp"

TEST(Alloc, Create)
{
  alloc allocator;
}

TEST(Alloc, SmallAllocation)
{
  alloc allocator;

  void *ptr = allocator.allocate (32);
  ASSERT_NE (ptr, nullptr);
}

TEST(Alloc, BigAllocation)
{
  alloc allocator;

  void *ptr = allocator.allocate (1 << 13);
  ASSERT_NE (ptr, nullptr);
}



TEST(Alloc, DifferentAllocations)
{
  alloc allocator (false);

  void *ptr1 = allocator.allocate (1 << 11);
  void *ptr2 = allocator.allocate (128);
  void *ptr3 = allocator.allocate (1 << 12);

  ASSERT_NE (ptr1, nullptr);
  ASSERT_NE (ptr2, nullptr);
  ASSERT_NE (ptr3, nullptr);
  ASSERT_NE (ptr1, ptr2);
  ASSERT_NE (ptr2, ptr3);
  ASSERT_NE (ptr3, ptr1);

  allocator.free (ptr1);
  allocator.free (ptr2);
  allocator.free (ptr3);
}


TEST(Alloc, TriggerCollect)
{
  alloc allocator (false);

  allocator.allocate (1 << 11);
  allocator.allocate (1 << 11);
  allocator.allocate (1 << 11);

  void *ptr1 = allocator.allocate (1 << 11);
  void *ptr2 = allocator.allocate (128);
  void *ptr3 = allocator.allocate (1 << 12);

  ASSERT_NE (ptr1, nullptr);
  ASSERT_NE (ptr2, nullptr);
  ASSERT_NE (ptr3, nullptr);

  allocator.free (ptr1);
  allocator.free (ptr2);
  allocator.free (ptr3);
}



TEST(Alloc, ManyAllocations)
{
  alloc allocator;
  auto n = 50;

  for (int i = 0; i < n; i++)
    {
      int *arr = (int*)allocator.allocate (256);
      arr[0] = 42;
    }
}


TEST(Alloc, GCSync)
{
  alloc allocator;
  std::atomic<int> joined (0);
  allocator.gc_barrier.add_new_thread();

  auto other_main = [&](int dummy) {
    allocator.gc_barrier.add_new_thread();
    if (!allocator.gc_barrier.finished_gc && allocator.gc_barrier.join())
      {
	allocator.gc_barrier.finish();
      }
    joined++;
  };
  
  auto other_thread = std::thread (other_main, 42);

  allocator.gc_barrier.finished_gc = false;
  if (!allocator.gc_barrier.finished_gc && allocator.gc_barrier.join())
    {
      allocator.gc_barrier.finish();
    }

  joined++;

  other_thread.join();

  ASSERT_EQ (joined, 2);
}
