#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "colours.h"
#include "foreach.h"
#include "util/symbol_visitor.h"
#include "util/extract_string.h"

struct resolve_vars
  : symbol_visitor
{
  void visit (t_roundvar &n);

  error_log &errors;

  resolve_vars (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , errors (annots.get ("errors"))
  {
  }

#if 0
  ~resolve_vars ()
  {
    if (!std::uncaught_exception ())
      exit (0);
  }
#endif
};

static phase<resolve_vars> thisphase ("resolve_vars", "insert_vardecl_syms", "insert_varadd_syms");


static std::string const &
id (node_ptr const &n)
{
  return n->as<token> ().string;
}


void
resolve_vars::visit (t_roundvar &n)
{
  std::string const &name = id (n.name ());
  generic_node_ptr sym = symtab.lookup (T_VARIABLE, name);

#if 0
  if (!sym)
    errors.add<semantic_error> (&n, "variable " + C::quoted (name) + " not declared in this scope");
#endif

  if (!sym)
    return;

  printf ("resolved %s:\n", name.c_str ());
  phases::run ("sx", sym);
}
