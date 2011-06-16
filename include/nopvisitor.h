#pragma once

#include "visitor.h"

struct nopvisitor
  : nodes::visitor
{
  virtual void visit (nodes::document &n);
  virtual void visit (nodes::definition_list &n);
  virtual void visit (nodes::token &n);
};
