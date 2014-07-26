#ifndef _UNWIND_HPP
#define _UNWIND_HPP

#define UNW_LOCAL_ONLY
#include <libunwind.h>

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
  void*
  stack_ptr()
  {
    unw_get_reg (&cursor, UNW_REG_SP, &sp);
    return (void*) sp;
  }

private:
  unw_cursor_t cursor;
  unw_context_t context;
  unw_word_t sp;
};

#endif
