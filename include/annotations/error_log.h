#pragma once

#include "annotation.h"
#include "exception.h"
#include "foreach.h"

#include <boost/ptr_container/ptr_vector.hpp>

namespace annotations
{
  struct error_log
    : annotation
  {
    typedef boost::ptr_vector<semantic_error> error_vec;

    error_vec log;

    template<typename Exception>
    void add (nodes::node_ptr const &node, std::string const &message)
    {
      log.push_back (new Exception (node, message));
    }

    bool has_errors () const
    {
      foreach (semantic_error const &e, log)
        if (e.error)
          return true;
      return false;
    }
  };
}
