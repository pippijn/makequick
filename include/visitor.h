#pragma once

#include "node_ptr.h"

namespace nodes
{
  struct visitor
  {
#include "visitor_t.h"

    virtual void visit (tokens::token &n);
    virtual void visit (generic_node &n);
  };
}
