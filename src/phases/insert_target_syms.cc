#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "util/make_var.h"
#include "util/symbol_visitor.h"

#include <stdexcept>

struct insert_target_syms
  : symbol_visitor
{
  void visit (t_target_definition &n);

  error_log &errors;

  insert_target_syms (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , errors (annots.get ("errors"))
  {
  }
};

static phase<insert_target_syms> thisphase ("insert_target_syms");


void
insert_target_syms::visit (t_target_definition &n)
{
  if (!symtab.insert (T_VARIABLE, "TARGET", make_var (id (n.name ()))))
    throw std::runtime_error ("unable to add $(TARGET) variable");

  if (!symtab.insert (node_type_to_symbol_type (current_symtype), "TARGET", &n))
    throw std::runtime_error ("unable to add $(TARGET) variable");
}
