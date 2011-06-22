#pragma once

#include "visitor.h"

struct nopvisitor
  : nodes::visitor
{
  virtual void visit (nodes::generic_node &n);
  virtual void visit (nodes::token &n);
};
