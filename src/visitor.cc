#include "phase.h"

using nodes::visitor;

#include "visitor_cc.h"

void
visitor::visit (tokens::token &n)
{
}

void
visitor::visit (generic_node &n)
{
  resume_list ();
}

bool
visitor::resume (node_ptr const &n)
{
  if (!n)
    return false;
  n->accept (*this);
  return true;
}
