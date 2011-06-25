#pragma once

#include "annotation.h"
#include "exception.h"

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
  };
}
