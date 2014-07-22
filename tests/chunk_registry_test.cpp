#include "../src/chunk_registry.hpp"
#include <gtest/gtest.h>

TEST(ChunkRegistry, AlignedSmall) {
  chunk_registry ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.new_chunk (32);

  ASSERT_EQ (ptr & TOP_BIT_MASK, ptr);
}

TEST(ChunkRegistry, AlignedLarge) {
  chunk_registry ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.new_chunk (1 << 13);

  ASSERT_EQ (ptr & TOP_BIT_MASK, ptr);
}

TEST(ChunkRegistry, AllocatedInRange) {
  chunk_registry ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.new_chunk_data (1 << 13);

  ASSERT_TRUE (ch_allocator.in_range ((void*)ptr));
}

TEST(ChunkRegistry, AfterMinimumChunkNotInRange) {
  chunk_registry ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.new_chunk_data (1 << 13);

  ASSERT_FALSE (ch_allocator.in_range ((void*)(ptr + (1 << 13))));
}

TEST(ChunkRegistry, FetchChunkObjectPtr) {
  chunk_registry ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.new_chunk_data (1 << 14);

  chunk* chunk = ch_allocator.find_chunk ((void*)ptr);

  ASSERT_NE (chunk, nullptr);
  ASSERT_EQ (chunk->data(), (void*)ptr);
}

TEST(ChunkRegistry, FetchHeaderInteriorPtr) {
  chunk_registry ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.new_chunk_data (1 << 14);

  chunk* chunk= ch_allocator.find_chunk ((void*)(ptr + (1 << 13)));

  ASSERT_EQ (chunk, nullptr);
}

TEST(ChunkRegistry, BackPointerIso) {
  chunk_registry ch_allocator;
  void *ptr = ch_allocator.new_chunk_data (1 << 14);
  chunk* chunk = ch_allocator.find_chunk (ptr);

  ASSERT_NE (chunk, nullptr);
  ASSERT_EQ (chunk->back_ptr->get(), chunk);
}

TEST(ChunkRegistry, MarkTrue) {
  chunk_registry ch_allocator;
  void *ptr = ch_allocator.new_chunk_data (1 << 14);

  ch_allocator.mark (ptr);
  ASSERT_TRUE (ch_allocator.is_marked (ptr));
}

TEST(ChunkRegistry, MarkFalse) {
  chunk_registry ch_allocator;
  void *ptr = ch_allocator.new_chunk_data (1 << 14);

  ASSERT_FALSE (ch_allocator.is_marked (ptr));
}

TEST(ChunkRegistry, BasePtrIsValid) {
  chunk_registry ch_allocator;
  void *ptr = ch_allocator.new_chunk_data (256);
  auto header = ch_allocator.find_chunk (ptr);

  ASSERT_NE (header, nullptr);
  ASSERT_TRUE (header->valid (ptr));
}


TEST(ChunkRegistry, NextPtrIsValid) {
  chunk_registry ch_allocator;
  void *ptr = ch_allocator.new_chunk_data (256);
  auto header = ch_allocator.find_chunk (ptr);

  ASSERT_NE (header, nullptr);
  ASSERT_TRUE (header->valid ((char*)ptr + 256UL));
}


TEST(ChunkRegistry, MidObjectInvalidPtr) {
  chunk_registry ch_allocator;
  void *ptr = ch_allocator.new_chunk_data (256);
  auto header = ch_allocator.find_chunk (ptr);

  ASSERT_NE (header, nullptr);
  ASSERT_FALSE (header->valid ((char*)ptr + 240UL));
}


TEST(ChunkRegistry, OutOfChunkInvalidPtr) {
  chunk_registry ch_allocator;
  void *ptr = ch_allocator.new_chunk_data (256);
  auto header = ch_allocator.find_chunk (ptr);

  ASSERT_NE (header, nullptr);
  ASSERT_FALSE (header->valid ((char*)ptr + 2*4096UL));
}
