#include <fstream>
#include "../src/unwind.hpp"
#include <gtest/gtest.h>
#include <stdio.h>

TEST(Unwind, IteratorBeginNotDone)
{
  register void **stack asm ("ebp");
  unwind_stack unw_stack (stack);
  ASSERT_NE (unw_stack.begin(), unw_stack.end());
}

TEST(Unwind, IteratorBeginEndNE)
{
  register void **stack asm ("ebp");
  unwind_stack unw_stack(stack);
  ASSERT_NE (unw_stack.begin(), unw_stack.end());
}

TEST(Unwind, FindSignalIterator)
{
  register void **stack asm ("ebp");
  unwind_stack unw_stack(stack);

  auto found_signal = false;

  std::size_t signal = 0xDEADBEEF;
  std::ofstream null ("/dev/null");
  null << &signal;

  for (auto it = unw_stack.begin(); it != unw_stack.end(); ++it)
    {
      found_signal |= (std::size_t)(*it) == signal;
    }

  ASSERT_TRUE (found_signal);
}

TEST(Unwind, FindSignalForSyntax)
{
  register void **stack asm ("ebp");
  unwind_stack unw_stack(stack);

  auto found_signal = false;

  std::size_t signal = 0xDEADBEEF;
  std::ofstream null ("/dev/null");

  null << &signal;

  for (auto ptr : unw_stack)
    {
      found_signal |= (std::size_t)ptr == signal;
    }

  ASSERT_TRUE (found_signal);
}
