#ifndef _UNWIND_HPP
#define _UNWIND_HPP

#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <iterator>

class unwind
{
public:
  inline
  void
  start()
  {
    unw_getcontext(&context);
    unw_init_local (&cursor, &context);
  }

  inline
  int
  step()
  {
    return unw_step (&cursor);
  }

  inline
  void**
  stack_ptr()
  {
    unw_get_reg (&cursor, UNW_REG_SP, &sp);
    return (void**) sp;
  }

private:
  unw_cursor_t cursor;
  unw_context_t context;
  unw_word_t sp;
};

class unwind_stack
{
public:
  class iterator : public std::iterator<std::forward_iterator_tag, void*>
  {
  public:
    typedef iterator self_type;
    iterator(bool set_to_bottom = false)
    {
      unwind unw;

      unw.start();
      current_ptr = unw.stack_ptr();

      if (set_to_bottom)
	{
	  while (unw.step() > 0)
	    {
	      current_ptr = unw.stack_ptr();
	    }
	}
    }

    iterator(const iterator &other) :
      current_ptr (other.current_ptr)
    {
    }

    self_type operator++()         { self_type i = *this ; current_ptr++ ; return i; }
    self_type operator++(int junk) { current_ptr++; return *this; }
    reference operator*() const    { 
      VALGRIND_MAKE_MEM_DEFINED (current_ptr, 8);
      return *current_ptr; 
    }
    pointer operator ->()          { return current_ptr; }
    bool operator==(const self_type& other) const
    {
      return
	current_ptr == other.current_ptr;
    }
    bool operator!=(const self_type& other) const { return !(*this == other); }

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
  unwind_stack () :
    end_it (true)
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
