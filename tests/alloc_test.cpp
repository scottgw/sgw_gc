#include <future>
#include <thread>
#include <gtest/gtest.h>
#include "../src/alloc.hpp"

TEST(Alloc, Create)
{
  register void **stack asm ("ebp");
  alloc allocator(stack);
}

TEST(Alloc, SmallAllocation)
{
  register void **stack asm ("ebp");
  alloc allocator(stack);

  void *ptr = allocator.allocate (32);
  ASSERT_NE (ptr, nullptr);
}

TEST(Alloc, BigAllocation)
{
  register void **stack asm ("ebp");
  alloc allocator(stack);

  void *ptr = allocator.allocate (1 << 13);
  ASSERT_NE (ptr, nullptr);
}



TEST(Alloc, DifferentAllocations)
{
  register void **stack asm ("ebp");
  alloc allocator (stack, false);

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
  register void **stack asm ("ebp");
  alloc allocator (stack, false);

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
  register void **stack asm ("ebp");
  alloc allocator(stack);
  auto n = 50;

  for (int i = 0; i < n; i++)
    {
      int *arr = (int*)allocator.allocate (256);
      arr[0] = 42;
    }
}


TEST(Alloc, GCSync)
{
  register void **stack asm ("ebp");
  alloc allocator(stack);
  std::atomic<int> joined (0);

  auto thread_main = [&](int dummy) {
    allocator.gc_barrier.add_thread();
    if (allocator.gc_barrier.trigger ())
      {
	allocator.gc_barrier.finish ();
      }

    joined++;
    allocator.gc_barrier.remove_thread();
  };
  
  auto other_thread = std::thread (thread_main, 42);
  thread_main (43);

  other_thread.join();

  ASSERT_EQ (joined, 2);
}
