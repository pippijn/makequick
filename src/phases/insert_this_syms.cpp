#include "phase.h"

#include "annotations/symbol_table.h"
#include "util/make_var.h"
#include "util/symbol_visitor.h"

struct insert_this_syms
  : symbol_visitor
{
  void visit (t_target_definition &n);

  insert_this_syms (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
  {
  }
};

static phase<insert_this_syms> thisphase ("insert_this_syms");


void
insert_this_syms::visit (t_target_definition &n)
{
  if (!symtab.insert (T_VARIABLE, "THIS", make_var ("$(builddir)/" + id (n.name ()) + "$(EXEEXT)")))
    throw std::runtime_error ("unable to add $(THIS) variable");
}
