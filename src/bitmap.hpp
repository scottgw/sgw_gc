#include <vector>

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
    auto major = pos / sizeof(char);
    auto minor = pos % sizeof(char);
    return (impl [major] & (1 << minor)) != 0;
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
