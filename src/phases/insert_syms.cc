#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "colours.h"
#include "foreach.h"
#include "util/symbol_visitor.h"

#include <stdexcept>

using annotations::error_log;
using annotations::symbol_table;

struct insert_syms
  : symbol_visitor
{
  void visit (t_vardecl &n);
  void visit (t_target_definition &n);

  error_log &errors;

  insert_syms (annotation_map &annots)
    : symbol_visitor (annots.put ("symtab", new symbol_table))
    , errors (annots.get ("errors"))
  {
  }

#if 0
  ~insert_syms ()
  {
    symtab.print ();
  }
#endif
};

static phase<insert_syms> thisphase ("insert_syms", "audit");


void
insert_syms::visit (t_vardecl &n)
{
  std::string const &name = n.var ()->as<token> ().string;
  generic_node_ptr sym = &n.body ()->as<t_vardecl_body> ();
  if (!symtab.insert (T_VARIABLE, name, sym))
    errors.add<semantic_error> (&n, "variable " + C::quoted (name) + " already defined in this scope");
}

void
insert_syms::visit (t_target_definition &n)
{
  symbol_type type;
  switch (state)
    {
    case S_PROGRAM:
      type = T_PROGRAM;
      break;
    case S_LIBRARY:
      type = T_LIBRARY;
      break;
    case S_TEMPLATE:
      type = T_TEMPLATE;
      break;
    default:
      throw std::runtime_error ("invalid state in target_definition");
    }
  std::string const &name = n.name ()->as<token> ().string;
  if (!symtab.insert (type, name, &n))
    {
      errors.add<semantic_error> (&n, "target definition " + C::quoted (name) + " already exists");
      return;
    }
  if (!symtab.insert (T_VARIABLE, "THIS", &n))
    throw std::runtime_error ("unable to add $(THIS) variable");

  symbol_visitor::visit (n);
}
