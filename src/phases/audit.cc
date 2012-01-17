#include "phase.h"

#include <stdexcept>

struct audit
  : visitor
{
  void visit (generic_node &n);

  std::vector<bool> seen;

  audit (annotation_map &annots)
    : seen (node::hash_size ())
  {
    if (!node::audit_hash ())
      throw std::runtime_error ("invalid node hash");
  }
};

static phase<audit> thisphase ("audit", noauto);


void 
audit::visit (generic_node &n)
{
  if (seen[n.index ()])
    throw semantic_error (&n, "cycle in abstract syntax tree");

  if (!n.parent ())
    {
      if (n.type != n_document)
        throw semantic_error (&n, "node has no parent");
    }
  else if ((*n.parent ())[n.parent_index ()] != &n)
    throw semantic_error (&n, "broken parent relationship");

  seen[n.index ()] = true;
  resume_list ();
  seen[n.index ()] = false;
}
