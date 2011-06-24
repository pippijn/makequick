#include "phase.h"

#include <set>

namespace
{
  struct audit
    : visitor
  {
    virtual void visit (token &n);
    virtual void visit (generic_node &n);

    std::set<void *> s;
  };

  static phase<audit> thisphase ("audit");
}

void
audit::visit (token &n)
{
}

void 
audit::visit (generic_node &n)
{
  if (should_terminate)
    return;
  //puts (semantic_error (&n, node_type_name[n.type]).what ());
  if (s.find (&n) != s.end ())
    throw semantic_error (&n, "cycle in abstract syntax tree");
  s.insert (&n);
  resume_list ();
  s.erase (&n);
}
