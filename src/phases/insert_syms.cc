#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "colours.h"
#include "foreach.h"

using annotations::error_log;
using annotations::symbol_table;

namespace
{
  struct insert_syms
    : visitor
  {
    void visit (t_vardecl &n);
    void visit (t_program &n);
    void visit (t_document &n);

    bool in_sources;
    error_log &errors;
    symbol_table &symtab;

    insert_syms (annotation_map &annots)
      : in_sources (false)
      , errors (annots.get ("errors"))
      , symtab (annots.put ("symtab", new symbol_table))
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
}


void
insert_syms::visit (t_vardecl &n)
{
  std::string const &name = n[0]->as<token> ().string;
  generic_node *sym = &n[1]->as<generic_node> ();
  if (!symtab.insert (T_VARIABLE, name, sym))
    errors.add<semantic_error> (&n, "variable " + C::quoted (name) + " already defined in this scope");
}

void
insert_syms::visit (t_program &n)
{
  symtab.enter_scope (&n);
  resume_list ();
  symtab.leave_scope ();
}

void
insert_syms::visit (t_document &n)
{
  symtab.enter_scope (&n);
  resume_list ();
  symtab.leave_scope ();
}
