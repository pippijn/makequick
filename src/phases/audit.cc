#include "phase.h"

struct audit
  : visitor
{
  void visit (generic_node &n);

  std::vector<bool> s;

  audit (annotation_map &annots)
    : s (node::node_count ())
  {
  }
};

static phase<audit> thisphase ("audit");


void 
audit::visit (generic_node &n)
{
  //puts (semantic_error (&n, node_type_name[n.type]).what ());
  if (s[n.index])
    throw semantic_error (&n, "cycle in abstract syntax tree");
  s[n.index] = true;
  resume_list ();
  s[n.index] = false;
}
