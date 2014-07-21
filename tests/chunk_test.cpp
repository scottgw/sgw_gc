#include "../src/chunk.hpp"
#include <gtest/gtest.h>

TEST(Chunk, AlignedSmall) {
  chunk_allocator ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.new_chunk (32);

  ASSERT_EQ (ptr & TOP_BIT_MASK, ptr);
}

TEST(Chunk, AlignedLarge) {
  chunk_allocator ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.new_chunk (1 << 13);

  ASSERT_EQ (ptr & TOP_BIT_MASK, ptr);
}

TEST(Chunk, AllocatedInRange) {
  chunk_allocator ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.new_chunk (1 << 13);

  ASSERT_TRUE (ch_allocator.in_range ((void*)ptr));
}

TEST(Chunk, AfterMinimumChunkNotInRange) {
  chunk_allocator ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.new_chunk (1 << 13);

  ASSERT_FALSE (ch_allocator.in_range ((void*)(ptr + (1 << 13))));
}

TEST(Chunk, FetchHeaderObjectPtr) {
  chunk_allocator ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.new_chunk (1 << 14);

  chunk_header* header = ch_allocator.find_chunk ((void*)ptr);

  ASSERT_NE (header, nullptr);
  ASSERT_EQ (header->data, (void*)ptr);
}

TEST(Chunk, FetchHeaderInteriorPtr) {
  chunk_allocator ch_allocator;
  intptr_t ptr = (intptr_t) ch_allocator.new_chunk (1 << 14);

  chunk_header* header = ch_allocator.find_chunk ((void*)(ptr + (1 << 13)));

  ASSERT_EQ (header, nullptr);
}

TEST(Chunk, BackPointerIso) {
  chunk_allocator ch_allocator;
  void *ptr = ch_allocator.new_chunk (1 << 14);
  chunk_header* header = ch_allocator.find_chunk (ptr);

  ASSERT_NE (header, nullptr);
  ASSERT_EQ (header->back_ptr->get(), header);
}

TEST(Chunk, MarkTrue) {
  chunk_allocator ch_allocator;
  void *ptr = ch_allocator.new_chunk (1 << 14);

  ch_allocator.mark (ptr);
  ASSERT_TRUE (ch_allocator.is_marked (ptr));
}

TEST(Chunk, MarkFalse) {
  chunk_allocator ch_allocator;
  void *ptr = ch_allocator.new_chunk (1 << 14);

  ASSERT_FALSE (ch_allocator.is_marked (ptr));
}
