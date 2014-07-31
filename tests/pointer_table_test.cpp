#include "../src/pointer_table.hpp"
#include <gtest/gtest.h>

class PointerTable : public ::testing::Test
{
  virtual
  void
  SetUp()
  {
    chnk = chunk::create(256);
    large_chnk = chunk::create(1 << 14);
  }

  virtual
  void
  TearDown()
  {
    chunk::destroy (chnk);
    chunk::destroy (large_chnk);
  }

protected:
  chunk *chnk;
  chunk *large_chnk;
  pointer_table ptable;
};

TEST_F(PointerTable, Create)
{
}

TEST_F(PointerTable, InsertLookup)
{
  ptable[chnk] = chnk;
  auto lookup = ptable[chnk];
  ASSERT_EQ (chnk, lookup);
}

TEST_F(PointerTable, InsertHas)
{
  ptable[chnk] = chnk;

  ASSERT_TRUE (ptable.has (chnk));
}

TEST_F(PointerTable, EmptyHasFalse)
{
  ASSERT_FALSE (ptable.has (chnk));
}


TEST_F(PointerTable, InsertBlankHasFalse)
{
  ptable[chnk] = chnk;
  ptable[chnk] = nullptr;
  ASSERT_FALSE (ptable.has (chnk));
}


TEST_F(PointerTable, InsertBlankSplit)
{
  ptable[chnk] = chnk;
  ptable[chnk] = nullptr;
  chunk *split = large_chnk->split (1 << 13);

  ASSERT_FALSE (ptable.has (chnk));
  ASSERT_FALSE (ptable.has (split));

  ptable[split] = split;
  ASSERT_TRUE (ptable.has (split));

  ptable[split] = nullptr;
  ASSERT_FALSE (ptable.has (split));

  large_chnk->merge (split);
}



TEST_F(PointerTable, LookupUninit)
{
  int misc;
  ASSERT_FALSE (ptable.has (&misc));
}
