#ifndef _BITMAP_HPP
#define _BITMAP_HPP

#include <vector>
#include <cassert>

struct bitmap
{
  bitmap (std::size_t size) :
    size (size),
    impl ((size / sizeof(char)) + 1, (char)0)
  {
  }

  bool
  operator [] (std::size_t pos)
  {
    assert (pos < size);
    auto major = pos / sizeof(char);
    auto minor = pos % sizeof(char);
    return (impl [major] & (1 << minor)) != 0;
  }

  bool
  all_clear()
  {
    for (auto b : impl)
      {
	if (b != 0)
	  {
	    return false;
	  }
      }

    return true;
  }

  void
  set (std::size_t pos, bool b)
  {
    auto major = pos / sizeof(char);
    auto minor = pos % sizeof(char);

    if (b)
      {
	impl [major] |= 1 << minor;
      }
    else
      {
	impl [major] &= ~(1 << minor);
      }
  }

  void
  clear()
  {
    for (auto &byte : impl)
      {
	byte = 0;
      }
  }

  std::size_t size;

private:
  std::vector<char> impl;
};

#endif
