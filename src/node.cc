#include "node.h"

namespace nodes
{
  void intrusive_ptr_release (node *n) { if (!--n->refcnt) delete n; }
  void intrusive_ptr_add_ref (node *n) { ++n->refcnt; }
}
