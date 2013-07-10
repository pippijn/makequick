#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/make_var.h"
#include "util/symbol_visitor.h"

#include "config.h"

#include <stdexcept>

struct insert_global_syms
  : symbol_visitor
{
  void visit (t_target_definition &n);

  error_log &errors;

  insert_global_syms (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , errors (annots.get ("errors"))
  {
    symtab.insert_global (T_VARIABLE, "__VERSION__", make_var (PACKAGE_STRING));
  }
};

static phase<insert_global_syms> thisphase ("insert_global_syms");


void
insert_global_syms::visit (t_target_definition &n)
{
  std::string const &name = id (n.name ());

  if (!symtab.insert_global (node_type_to_symbol_type (current_symtype), name, &n))
    errors.add<semantic_error> (&n, "target definition " + C::quoted (name) + " already exists");
}
