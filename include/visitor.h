#pragma once

namespace nodes
{
  struct visitor
  {
    virtual void visit (struct document &n) { }
    virtual void visit (struct definition_list &n) { }
    virtual void visit (struct token &n) { }
  };
}
