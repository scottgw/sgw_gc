#ifndef _UNWIND_HPP
#define _UNWIND_HPP

#include <iterator>

class unwind_stack
{
public:
  class iterator : public std::iterator<std::forward_iterator_tag, void*>
  {
  public:
    typedef iterator self_type;

    inline
    iterator(bool set_to_bottom = false, pointer bottom = nullptr)
    {
      if (set_to_bottom)
	{
	  current_ptr = bottom;
	}
      else
	{
	  register void** esp asm("esp");
	  current_ptr = esp;
	}
    }

    iterator(const iterator &other) :
      current_ptr (other.current_ptr)
    {
    }

    self_type operator++()
    {
      self_type i = *this;
      current_ptr++;
      return i;
    }

    self_type operator++(int junk)
    {
      current_ptr++;
      return *this;
    }

    reference operator*() const
    {
      return *current_ptr;
    }

    pointer operator ->()
    {
      return current_ptr;
    }

    bool operator==(const self_type& other) const
    {
      return current_ptr == other.current_ptr;
    }

    bool operator!=(const self_type& other) const
    {
      return !(*this == other);
    }

    iterator& operator= (const iterator& other)
    {
      if (this != &other)
	{
	  current_ptr = other.current_ptr;
	}

      return *this;
    }

  private:
    pointer current_ptr;
  };

public:
  unwind_stack (void** stack_bottom) :
    end_it (true, stack_bottom)
  {
  }

  iterator
  begin()
  {
    iterator it;
    return it;
  }

  iterator
  end()
  {
    return end_it;
  }

private:
  iterator end_it;
};

#endif
