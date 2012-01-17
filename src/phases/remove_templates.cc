#include "phase.h"

#include "annotations/symbol_table.h"
#include "util/symbol_visitor.h"

#include <stdexcept>

struct remove_templates
  : symbol_visitor
{
  void visit (t_template &n);

  remove_templates (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
  {
  }
};

static phase<remove_templates> thisphase ("remove_templates", "inheritance");


void
remove_templates::visit (t_template &n)
{
  t_target_definition &defn = n.defn ()->as<t_target_definition> ();
  symtab.remove (T_TEMPLATE, defn.name ()->as<token> ().string, &defn);
  n.parent ()->list[n.parent_index ()] = NULL;

  symbol_visitor::visit (n);
}
