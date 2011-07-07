#include "util/symbol_visitor.h"
#include "util/resume.h"
#include "util/local.h"

using namespace nodes;

void
symbol_visitor::visit (t_target_definition &n)
{
  symtab.enter_scope (&n);
  resume_list ();
  symtab.leave_scope ();
}

void
symbol_visitor::visit (t_program &n)
{
  local (state) = S_PROGRAM;
  visitor::visit (n);
}

void
symbol_visitor::visit (t_library &n)
{
  local (state) = S_LIBRARY;
  visitor::visit (n);
}

void
symbol_visitor::visit (t_template &n)
{
  local (state) = S_TEMPLATE;
  visitor::visit (n);
}

void
symbol_visitor::visit (t_document &n)
{
  symtab.enter_scope (&n);
  visitor::visit (n);
  symtab.leave_scope ();
}
