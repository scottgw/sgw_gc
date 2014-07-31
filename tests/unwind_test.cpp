#include <fstream>
#include "../src/unwind.hpp"
#include <gtest/gtest.h>
#include <stdio.h>


TEST(Unwind, Start)
{
  unwind unw;
  unw.start();
  ASSERT_NE (unw.stack_ptr(), nullptr);
};


TEST(Unwind, FindSignal)
{
  std::ofstream null ("/dev/null");
  unwind unw;
  std::size_t signal = 0xDEADBEEF;
  null << &signal;
  unw.start();

  auto curr_stack_ptr = (void**)unw.stack_ptr();

  // Walk up stack looking for our signal
  auto found_signal = false;
  while (unw.step())
    {
      auto next_stack_ptr = (void**)unw.stack_ptr();
      for (auto ptr = curr_stack_ptr; ptr <= next_stack_ptr; ptr++)
	{
	  std::size_t val = *((std::size_t*)ptr);
	  found_signal |= val == signal;
	}
      curr_stack_ptr = next_stack_ptr;
    }
  ASSERT_TRUE (found_signal);
};


TEST(Unwind, IteratorEndIsDone)
{
  unwind_stack stack;
  ASSERT_TRUE (stack.end().is_done());
}


TEST(Unwind, IteratorBeginNotDone)
{
  unwind_stack stack;
  ASSERT_FALSE (stack.begin().is_done());
}


TEST(Unwind, IteratorBeginEndNE)
{
  unwind_stack stack;
  ASSERT_NE (stack.begin(), stack.end());
}

TEST(Unwind, FindSignalIterator)
{
  unwind_stack stack;
  auto found_signal = false;

  std::size_t signal = 0xDEADBEEF;
  std::ofstream null ("/dev/null");
  null << &signal;

  for (auto it = stack.begin(); it != stack.end(); ++it)
    {
      found_signal |= (std::size_t)(*it) == signal;
    }

  ASSERT_TRUE (found_signal);
};


TEST(Unwind, FindSignalForSyntax)
{
  unwind_stack stack;
  auto found_signal = false;

  std::size_t signal = 0xDEADBEEF;
  std::ofstream null ("/dev/null");
  null << &signal;

  for (auto ptr : stack)
    {
      found_signal |= (std::size_t)ptr == signal;
    }

  ASSERT_TRUE (found_signal);
};
