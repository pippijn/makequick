#pragma once

#include <vector>

#include <boost/intrusive_ptr.hpp>

#define NODE(T) struct T; typedef boost::intrusive_ptr<T> T##_ptr; typedef std::vector<T##_ptr> T##_vec

struct location;

namespace nodes
{
  NODE (node);
  void intrusive_ptr_release (node *n);
  void intrusive_ptr_add_ref (node *n);

  struct node_hash
  {
    size_t operator () (node_ptr n) const;
  };

  struct node_eq
  {
    bool operator () (node_ptr n1, node_ptr n2) const;
  };

  NODE (node_list);
  NODE (generic_node);
}

namespace tokens
{
  NODE (token);

  template<short Tok>
  token *make_token (char const *text, int leng);
}

#undef NODE

template<typename T>
boost::intrusive_ptr<T>
move (boost::intrusive_ptr<T> &p)
{
  boost::intrusive_ptr<T> r;
  swap (r, p);
  return r;
}

using nodes::node;
using nodes::node_ptr;

namespace nodes
{
#define NODE(T) struct t_##T; typedef boost::intrusive_ptr<t_##T> t_##T##_ptr; typedef std::vector<t_##T##_ptr> t_##T##_vec;
#include "node_fwd.h"
#undef NODE
}
