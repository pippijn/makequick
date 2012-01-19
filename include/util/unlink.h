#pragma once

#include "node.h"

static inline nodes::node_vec
unlink_all (nodes::node_vec &v)
{
  nodes::node_vec r;
  r.reserve (v.size ());
  foreach (node_ptr &p, v)
    if (p)
      {
        assert (p->parent_index () < v.size ());
        r.push_back (p);
        p->unlink ();
      }
  v.clear ();
  return r;
}
