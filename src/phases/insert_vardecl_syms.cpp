#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/make_var.h"
#include "util/symbol_visitor.h"

#include <stdexcept>
#include <boost/filesystem/path.hpp>

struct insert_vardecl_syms
  : symbol_visitor
{
  void visit (t_vardecl &n);
  void visit (t_target_definition &n);

  error_log &errors;

  insert_vardecl_syms (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , errors (annots.get ("errors"))
  {
  }

#if 0
  ~insert_vardecl_syms ()
  {
    symtab.print ();
    throw 0;
  }
#endif
};

static phase<insert_vardecl_syms> thisphase ("insert_vardecl_syms", "inheritance");


void
insert_vardecl_syms::visit (t_vardecl &n)
{
  std::string const &name = id (n.var ());
  generic_node_ptr sym = &n.body ()->as<t_vardecl_body> ();
  if (!symtab.insert (T_VARIABLE, name, sym))
    errors.add<semantic_error> (&n, "variable " + C::quoted (name) + " already defined in this scope");
}


void
insert_vardecl_syms::visit (t_target_definition &n)
{
  if (!symtab.insert (T_VARIABLE, "CURDIR", make_var (n.loc.file->parent_path ().native ())))
    throw std::runtime_error ("unable to add $(CURDIR) variable");

  symbol_visitor::visit (n);
}
