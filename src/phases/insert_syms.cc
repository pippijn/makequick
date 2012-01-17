#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "colours.h"
#include "foreach.h"
#include "util/symbol_visitor.h"

#include <stdexcept>

struct insert_syms
  : symbol_visitor
{
  void visit (t_vardecl &n);
  void visit (t_varadd &n);
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
    throw 0;
  }
#endif
};

static phase<insert_syms> thisphase ("insert_syms");


void
insert_syms::visit (t_vardecl &n)
{
  std::string const &name = n.var ()->as<token> ().string;
  generic_node_ptr sym = &n.body ()->as<t_vardecl_body> ();
  if (!symtab.insert (T_VARIABLE, name, sym))
    errors.add<semantic_error> (&n, "variable " + C::quoted (name) + " already defined in this scope");
}

void
insert_syms::visit (t_varadd &n)
{
  std::string const &name = n.var ()->as<token> ().string;
  generic_node_ptr sym = &n.body ()->as<t_vardecl_body> ();
  if (!symtab.insert_global (T_VARIABLE, name, sym))
    errors.add<semantic_error> (&n, "variable " + C::quoted (name) + " already defined in this scope");
}


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
insert_syms::visit (t_target_definition &n)
{
  symbol_type type = node_type_to_symbol_type (current_symtype);

  std::string const &name = n.name ()->as<token> ().string;

  bool success = symtab.insert_global (type, name, &n);

  if (!success)
    {
      errors.add<semantic_error> (&n, "target definition " + C::quoted (name) + " already exists");
      return;
    }
  if (!symtab.insert (type, "TARGET", &n))
    throw std::runtime_error ("unable to add $(TARGET) variable");

  symbol_visitor::visit (n);
}
