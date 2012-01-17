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

#define resume_list()								\
  for (std::vector<node_ptr>::iterator it = n.list.begin (), et = n.list.end ();\
       it != et; ++it)								\
    resume (*it)
