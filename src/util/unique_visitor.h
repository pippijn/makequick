#pragma once

#include "node.h"

struct unique_visitor
{
  bool done (generic_node const &n) { return at (n.index ()); }
  void mark (generic_node const &n) { at (n.index ()) = true; }

  unique_visitor ()
    : seen (node::hash_size ())
  {
  }

private:
  std::vector<bool>::reference at (size_t index)
  {
    if (seen.size () <= index)
      seen.resize (index + 1);
    return seen.at (index);
  }

  std::vector<bool> seen;
};
