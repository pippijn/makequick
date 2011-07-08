#include "node.h"

nodes::node_list *
nodes::make_node (nodes::node_type type)
{
  switch (type)
    {
#define NODE(T) case n_##T: return new t_##T;
#include "node_fwd.h"
#undef NODE
    }
  throw 0;
}
