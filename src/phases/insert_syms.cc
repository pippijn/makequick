#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "colours.h"
#include "foreach.h"

#include <stdexcept>

using annotations::error_log;
using annotations::symbol_table;

struct insert_syms
  : visitor
{
  void visit (t_vardecl &n);
  void visit (t_target_definition &n);
  void visit (t_program &n);
  void visit (t_library &n);
  void visit (t_template &n);
  void visit (t_document &n);

  error_log &errors;
  symbol_table &symtab;

  enum visit_state
  {
    S_NONE,
    S_PROGRAM,
    S_LIBRARY,
    S_TEMPLATE,
  } state;

  insert_syms (annotation_map &annots)
    : errors (annots.get ("errors"))
    , symtab (annots.put ("symtab", new symbol_table))
    , state (S_NONE)
  {
  }

#if 1
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
  symtab.insert (type, n.name ()->as<token> ().string, &n);

  symtab.enter_scope (&n);
  resume_list ();
  symtab.leave_scope ();
}

void
insert_syms::visit (t_program &n)
{
  local (state) = S_PROGRAM;
  visitor::visit (n);
}

void
insert_syms::visit (t_library &n)
{
  local (state) = S_LIBRARY;
  visitor::visit (n);
}

void
insert_syms::visit (t_template &n)
{
  local (state) = S_TEMPLATE;
  visitor::visit (n);
}

void
insert_syms::visit (t_document &n)
{
  symtab.enter_scope (&n);
  visitor::visit (n);
  symtab.leave_scope ();
}
