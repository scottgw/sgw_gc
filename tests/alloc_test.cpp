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


TEST(Alloc, TriggerCollect)
{
  auto allocator = new alloc();

  void *ptr1 = allocator->allocate (1 << 11);
  ptr1 = allocator->allocate (1 << 11);
  ptr1 = allocator->allocate (1 << 11);
  ptr1 = allocator->allocate (1 << 11);
  void *ptr2 = allocator->allocate (128);
  void *ptr3 = allocator->allocate (1 << 12);

  ASSERT_NE (ptr1, nullptr);
  ASSERT_NE (ptr2, nullptr);
  ASSERT_NE (ptr3, nullptr);

  allocator->free (ptr1);
  allocator->free (ptr2);
  allocator->free (ptr3);
}
