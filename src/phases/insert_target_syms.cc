#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/symbol_visitor.h"

#include <stdexcept>

struct insert_target_syms
  : symbol_visitor
{
  void visit (t_target_definition &n);

  error_log &errors;

  insert_target_syms (annotation_map &annots)
    : symbol_visitor (annots.put ("symtab", new symbol_table))
    , errors (annots.get ("errors"))
  {
  }

#if 0
  ~insert_target_syms ()
  {
    symtab.print ();
    throw 0;
  }
#endif
};

static phase<insert_target_syms> thisphase ("insert_target_syms");


static symbol_type
node_type_to_symbol_type (node_type type)
{
  switch (type)
    {
    case n_program : return T_PROGRAM;
    case n_library : return T_LIBRARY;
    case n_template: return T_TEMPLATE;
    }
  throw std::runtime_error ("invalid state in target_definition");
}

void
insert_target_syms::visit (t_target_definition &n)
{
  symbol_type type = node_type_to_symbol_type (current_symtype);

  std::string const &name = n.name ()->as<token> ().string;

  bool success = symtab.insert_global (type, name, &n);

  if (!success)
    {
      errors.add<semantic_error> (&n, "target definition " + C::quoted (name) + " already exists");
      return;
    }

  symbol_visitor::visit (n);
}
