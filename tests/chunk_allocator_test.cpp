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

TEST_F(ChunkAllocator, SmallAfterLarge1) {
  chunk_allocator ch_alloc;
  auto large1 = ch_alloc.allocate (1 << 11);
  ASSERT_NE (large1, nullptr);
  auto large2 = ch_alloc.allocate (1 << 11);
  ASSERT_NE (large2, nullptr);
  auto large3 = ch_alloc.allocate (1 << 11);
  ASSERT_NE (large3, nullptr);
  auto med = ch_alloc.allocate (256);
  ASSERT_NE (med, nullptr);

  ASSERT_EQ (med->object_size, 256);
}


TEST_F(ChunkAllocator, SmallAfterLarge2) {
  chunk_allocator ch_alloc;
  auto large1 = ch_alloc.allocate (1 << 11);
  ASSERT_NE (large1, nullptr);
  auto large2 = ch_alloc.allocate (1 << 11);
  ASSERT_NE (large2, nullptr);
  auto large3 = ch_alloc.allocate (1 << 11);
  ASSERT_NE (large3, nullptr);
  auto large4 = ch_alloc.allocate (1 << 11);
  ASSERT_NE (large4, nullptr);

  auto med = ch_alloc.allocate (256);
  ASSERT_NE (med, nullptr);

  ASSERT_EQ (med->object_size, 256);
}
