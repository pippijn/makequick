#pragma once

namespace nodes
{
  struct visitor
  {
    virtual void visit (struct generic_node &n) { }
    virtual void visit (struct token &n) { }
  };
}
