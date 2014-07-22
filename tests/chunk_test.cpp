#include <gtest/gtest.h>
#include "../src/chunk.hpp"

TEST (Chunk, Create)
{
  chunk *chunk = chunk::create(256);  
  ASSERT_NE (chunk, nullptr);
}

TEST (Chunk, EndAfterBegin)
{
  chunk *chunk = chunk::create(256);
  ASSERT_LE ((std::size_t)chunk, (std::size_t)chunk->end());
}

TEST (Chunk, EngAligned)
{
  chunk *chunk = chunk::create(256);
  ASSERT_EQ ((std::size_t)chunk->end() % CHUNK_SIZE, 0);
}

TEST (Chunk, EndBeginDiffAligned)
{
  chunk *chunk = chunk::create(256);
  auto diff = (std::size_t)chunk->end() - (std::size_t)chunk; 
  ASSERT_EQ (diff % CHUNK_SIZE, 0);
}

TEST (Chunk, Split)
{
  chunk *chnk = chunk::create(1 << 14);
  auto old_end = chnk->end();
  chunk *split_chnk = chnk->split(256);

  ASSERT_LT (chnk, split_chnk);
  ASSERT_EQ (split_chnk->end(), old_end);
  ASSERT_EQ ((std::size_t)split_chnk->end() % CHUNK_SIZE, 0);
  ASSERT_EQ ((std::size_t)chnk->end() % CHUNK_SIZE, 0);
}

TEST (Chunk, SplitMerge)
{
  chunk *chnk = chunk::create(1 << 14);
  auto old_end = chnk->end();
  auto old_size = chnk->size();
  chunk *split_chnk = chnk->split(256);
  
  chnk->merge (split_chnk);
  ASSERT_EQ (chnk->end(), old_end);
  ASSERT_EQ (chnk->size(), old_size);
}
