#pragma once

#include "node_ptr.h"

namespace nodes
{
  struct visitor
  {
#include "visitor_t.h"

    virtual void visit (tokens::token &n);
    virtual void visit (generic_node &n);

    bool resume (node_ptr const &n);
  };
}

#define resume_list()				\
  for (size_t _i = 0; _i < n.size (); _i++)	\
    resume (n[_i])
