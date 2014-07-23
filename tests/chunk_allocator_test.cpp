#include "../src/chunk_allocator.hpp"
#include <gtest/gtest.h>

class ChunkAllocator : public ::testing::Test {
public:
  virtual
  void
  SetUp ()
  {
    small_chunk = ch_alloc.allocate (256);
    large_chunk = ch_alloc.allocate (1 << 13);
  }

  virtual
  void
  TearDown()
  {
    ch_alloc.free (small_chunk);
    ch_alloc.free (large_chunk);
  }

  chunk* small_chunk;
  chunk* large_chunk;

  chunk_allocator ch_alloc;
};

TEST_F(ChunkAllocator, AlignedSmall) {
  ASSERT_EQ ((intptr_t)small_chunk & TOP_BIT_MASK, (intptr_t)small_chunk);
}

TEST_F(ChunkAllocator, AlignedLarge) {
  ASSERT_EQ ((intptr_t)large_chunk & TOP_BIT_MASK, (intptr_t)large_chunk);
}

