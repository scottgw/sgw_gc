#include <gtest/gtest.h>
#include "../src/chunk.hpp"

class Chunk : public ::testing::Test
{
protected:
  virtual
  void
  SetUp()
  {
    small_chunk = chunk::create (256);
    large_chunk = chunk::create (1 << 14);

    small_ptr = small_chunk->data();
    large_ptr = large_chunk->data();
  }

  virtual
  void
  TearDown()
  {
    chunk::destroy (small_chunk);
    chunk::destroy (large_chunk);
  }

  void *small_ptr;
  void *large_ptr;
  chunk *small_chunk;
  chunk *large_chunk;
};


TEST_F (Chunk, Create)
{
  ASSERT_NE (small_chunk, nullptr);
}

TEST_F (Chunk, EndAfterBegin)
{
  ASSERT_LE ((std::size_t)small_chunk, (std::size_t)small_chunk->end());
}

TEST_F (Chunk, EngAligned)
{
  ASSERT_EQ ((std::size_t)small_chunk->end() % CHUNK_SIZE, 0);
}

TEST_F (Chunk, EndBeginDiffAligned)
{
  auto diff = (std::size_t)small_chunk->end() - (std::size_t)small_chunk; 
  ASSERT_EQ (diff % CHUNK_SIZE, 0);
}

TEST_F (Chunk, Split)
{
  auto old_end = large_chunk->end();
  chunk *split_chnk = large_chunk->split(256);

  ASSERT_LT (large_chunk, split_chnk);
  ASSERT_EQ (split_chnk->end(), old_end);
  ASSERT_EQ ((std::size_t)split_chnk->end() % CHUNK_SIZE, 0);
  ASSERT_EQ ((std::size_t)large_chunk->end() % CHUNK_SIZE, 0);
}

TEST_F (Chunk, SplitMerge)
{
  auto old_end = large_chunk->end();
  auto old_size = large_chunk->size();
  chunk *split_chnk = large_chunk->split(256);
  
  large_chunk->merge (split_chnk);
  ASSERT_EQ (large_chunk->end(), old_end);
  ASSERT_EQ (large_chunk->size(), old_size);
}
