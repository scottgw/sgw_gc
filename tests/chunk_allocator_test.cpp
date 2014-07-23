#include "../src/chunk_allocator.hpp"
#include <gtest/gtest.h>

TEST(ChunkAllocator, AlignedSmall) {
  chunk_allocator ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.allocate (32);

  ASSERT_EQ (ptr & TOP_BIT_MASK, ptr);
}

TEST(ChunkAllocator, AlignedLarge) {
  chunk_allocator ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.allocate (1 << 13);

  ASSERT_EQ (ptr & TOP_BIT_MASK, ptr);
}

