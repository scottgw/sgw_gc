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


static
int
count_true (bitmap &bitmap, int count)
{
  auto sum = 0;
  for (int i = 0; i < count; i++)
    {
      if (bitmap[i])
	{
	  sum++;
	}
    }
  return sum;
}

static
void
set_all (bitmap &bitmap, int count)
{
  for (int i = 0; i < count; i++)
    {
      bitmap.set (i, true);
    }
}


TEST(Bitmap, Creation)
{
  bitmap bitmap(1025);  
}

TEST(Bitmap, SetTrue)
{
  bitmap bitmap(1025);
  bitmap.set (1024, true);
}

TEST(Bitmap, SetFalse)
{
  bitmap bitmap(1025);
  set_all (bitmap, 1025);
  bitmap.set (1024, false);
  ASSERT_FALSE (bitmap[1024]);
  ASSERT_EQ (count_true (bitmap, 1025), 1024);
}

TEST(Bitmap, Get)
{
  bitmap bitmap(1025);
  bitmap.set (1024, true);
  ASSERT_TRUE (bitmap [1024]);
}

TEST(Bitmap, Clear)
{
  bitmap bitmap(1025);

  set_all (bitmap, 1025);
  bitmap.clear();

  ASSERT_EQ (count_true (bitmap, 1025), 0);
}
