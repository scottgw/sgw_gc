#include "../src/unwind.hpp"
#include <gtest/gtest.h>
#include <stdio.h>

TEST(Unwind, Create)
{
  unwind unw;
};

TEST(Unwind, Start)
{
  unwind unw;
  unw.start();
  ASSERT_NE (unw.stack_ptr(), nullptr);
};


TEST(Unwind, FindSignal)
{
  unwind unw;
  std::size_t signal = 0xDEADBEEF;
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
