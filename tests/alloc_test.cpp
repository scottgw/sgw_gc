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
  allocator.free (ptr);
}

TEST(Alloc, BigAllocation)
{
  alloc allocator;

  void *ptr = allocator.allocate (1 << 13);
  ASSERT_NE (ptr, nullptr);
  allocator.free (ptr);
}
