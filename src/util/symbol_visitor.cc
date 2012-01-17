#include "util/symbol_visitor.h"
#include "util/resume.h"
#include "util/local.h"

#include "annotations/symbol_table.h"

using namespace nodes;

symbol_visitor::symbol_visitor (symbol_table &symtab)
  : symtab (symtab)
  , current_symtype (n_document)
{
}

static bool
has_scope (node_type type)
{
  switch (type)
    {
    case n_program:
    case n_template:
    case n_library:
       return true;
    default:
       return false;
    }
}

void
symbol_visitor::visit (generic_node &n)
{
  local (current_symtype);
  node_type const type = node_type (n.type);
  if (has_scope (type))
    {
      current_symtype = type;
      symtab.enter_scope (&n);
    }
  resume_list ();
  if (has_scope (type))
    symtab.leave_scope ();
}
