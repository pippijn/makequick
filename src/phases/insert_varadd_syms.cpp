#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/symbol_visitor.h"

#include <stdexcept>

struct insert_varadd_syms
  : symbol_visitor
{
  void visit (t_varadd &n);

  error_log &errors;

  insert_varadd_syms (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , errors (annots.get ("errors"))
  {
  }

#if 0
  ~insert_varadd_syms ()
  {
    symtab.print ();
    throw 0;
  }
#endif
};

static phase<insert_varadd_syms> thisphase ("insert_varadd_syms", "inheritance");


void
insert_varadd_syms::visit (t_varadd &n)
{
#if 0
  std::string const &name = n.var ()->as<token> ().string;
  generic_node_ptr sym = &n.body ()->as<t_vardecl_body> ();
  if (!symtab.insert_global (T_VARIABLE, name, sym))
    errors.add<semantic_error> (&n, "variable " + C::quoted (name) + " already defined in this scope");
#endif
}
