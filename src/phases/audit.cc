#include "phase.h"

#include <stdexcept>

struct audit
  : visitor
{
  void visit (generic_node &n);

  std::vector<bool> s;
  bool saw_root;

  audit (annotation_map &annots)
    : s (node::hash_size ())
    , saw_root (false)
  {
    if (!node::audit_hash ())
      throw std::runtime_error ("invalid node hash");
  }
};

static phase<audit> thisphase ("audit", noauto);


void 
audit::visit (generic_node &n)
{
  if (s[n.index])
    throw semantic_error (&n, "cycle in abstract syntax tree");

  if (!n.parent)
    {
      if (saw_root)
        throw semantic_error (&n, "node has no parent");
      saw_root = true;
    }
  else if ((*n.parent)[n.parent_index] != &n)
    throw semantic_error (&n, "broken parent relationship");

  s[n.index] = true;
  resume_list ();
  s[n.index] = false;
}
