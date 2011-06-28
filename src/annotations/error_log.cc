#include "annotations/error_log.h"

#include "foreach.h"

#include <cstdio>

#include <boost/ptr_container/ptr_vector.hpp>

namespace annotations
{
  struct error_log::pimpl
  {
    typedef boost::ptr_vector<semantic_error> error_vec;

    error_vec log;
  };

  error_log::error_log ()
    : self (new pimpl)
  {
  }

  error_log::~error_log ()
  {
  }

  void
  error_log::add (semantic_error *e)
  {
    self->log.push_back (e);
  }

  bool
  error_log::has_errors () const
  {
    foreach (semantic_error const &e, self->log)
      if (e.error)
        return true;
    return false;
  }

  bool
  error_log::has_diagnostics () const
  {
    return !self->log.empty ();
  }

  void
  error_log::print () const
  {
    int count = 0;
    foreach (semantic_error const &e, self->log)
      {
        if (count++ > 10)
          {
            puts ("%% too many diagnostics; exiting");
            break;
          }
        puts (e.what ());
      }
  }
}
