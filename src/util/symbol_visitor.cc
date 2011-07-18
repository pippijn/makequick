#include "util/symbol_visitor.h"
#include "util/resume.h"
#include "util/local.h"

using namespace nodes;

void
symbol_visitor::visit (t_target_definition &n)
{
  //symtab.enter_scope (&n);
  resume_list ();
  //symtab.leave_scope ();
}

void
symbol_visitor::visit (t_program &n)
{
  local (state) = S_PROGRAM;
  symtab.enter_scope (&n);
  visitor::visit (n);
  symtab.leave_scope ();
}

void
symbol_visitor::visit (t_library &n)
{
  local (state) = S_LIBRARY;
  symtab.enter_scope (&n);
  visitor::visit (n);
  symtab.leave_scope ();
}

void
symbol_visitor::visit (t_template &n)
{
  local (state) = S_TEMPLATE;
  symtab.enter_scope (&n);
  visitor::visit (n);
  symtab.leave_scope ();
}

void
symbol_visitor::visit (t_toplevel_declarations &n)
{
  symtab.enter_scope (&n);
  visitor::visit (n);
  symtab.leave_scope ();
}
