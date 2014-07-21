#include "../src/bitmap.hpp"
#include <gtest/gtest.h>

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
