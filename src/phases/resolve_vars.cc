#include "phase.h"

#include "annotations/symbol_table.h"
#include "util/foreach.h"
#include "util/symbol_visitor.h"
#include "util/extract_string.h"
#include "util/unique_visitor.h"

struct resolve_vars
  : symbol_visitor
  , unique_visitor
{
  void visit (t_roundvar &n);
  bool override (generic_node &n);

  generic_node_ptr sym;

  resolve_vars (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
  {
  }
};

static phase<resolve_vars> thisphase ("resolve_vars", "insert_vardecl_syms", "insert_varadd_syms");


static std::string const &
id (node_ptr const &n)
{
  return n->as<token> ().string;
}


void
resolve_vars::visit (t_roundvar &n)
{
  std::string const &name = id (n.name ());
  sym = symtab.lookup (T_VARIABLE, name);
}


bool
resolve_vars::override (generic_node &n)
{
  if (done (n))
    return true;
  mark (n);

  local (sym);
  foreach (node_ptr const &p, n.list)
    {
      if (!p)
        continue;

      sym = NULL;
      p->accept (*this);

      if (sym)
        {
          if (!done (*sym))
            sym->accept (*this);
          node_ptr body = sym->as<t_vardecl_body> ().clone ();
          n.set (p->parent_index (), body);
        }
    }

  return true;
}
