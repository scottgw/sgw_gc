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
  bool
  step()
  {
    return unw_step (&cursor) > 0;
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
    iterator()
    {
      done = false;
      unw.start();
      current_ptr = unw.stack_ptr();
      unw.step();
      next_ptr = unw.stack_ptr();
    }

    iterator(const iterator &other) :
      unw (other.unw),
      current_ptr (other.current_ptr),
      next_ptr (other.next_ptr),
      done (other.done)
    {
    }

    self_type operator++()         { self_type i = *this ; incr() ; return i; }
    self_type operator++(int junk) { incr(); return *this; }
    reference operator*() const    { return *current_ptr; }
    pointer operator ->()          { return current_ptr; }
    bool operator==(const self_type& other) const
    {
      return
	(done && other.done) ||
	(done == other.done && 
	 current_ptr == other.current_ptr &&
	 next_ptr == other.next_ptr);
    }
    bool operator!=(const self_type& other) const { return !(*this == other); }

    iterator& operator= (const iterator& other)
    {
      if (this != &other)
	{
	  unw = other.unw;
	  current_ptr = other.current_ptr;
	  next_ptr = other.next_ptr;
	  done = other.done;
	}

      return *this;
    }
    
    void set_done () { done = true; }
    bool is_done () { return done; }
  private:
    void
    incr()
    {
      current_ptr++;
      if (current_ptr > next_ptr)
	{
	  step();
	}
    };

    void
    step()
    {
      done = !unw.step();
      if (!done)
	{
	  current_ptr = next_ptr++;
	  next_ptr = unw.stack_ptr();
	}
    }

    unwind unw;
    pointer current_ptr;
    pointer next_ptr;
    bool done;
  };

public:
  iterator
  begin()
  {
    iterator it;
    return it;
  }

  iterator
  end()
  {
    iterator it;
    it.set_done();
    return it;
  }
};

#endif
