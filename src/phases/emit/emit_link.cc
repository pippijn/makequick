#include "phase.h"

#include "annotations/output_file.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/symbol_visitor.h"

#include <stdexcept>

#include "canonical.h"

struct emit_link
  : symbol_visitor
{
  virtual void visit (t_link_body &n);

  virtual void visit (token &n);

  output_file const &out;

  emit_link (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , out (annots.get ("output"))
  {
  }
};

static phase<emit_link> thisphase ("emit_link", "emit");


static char const *
ldadd (node_type type)
{
  if (type == n_library)
    return "LIBADD";
  if (type == n_program)
    return "LDADD";
  assert (!"invalid");
}

void
emit_link::visit (t_link_body &n)
{
  t_target_definition &target = symtab.lookup<t_target_definition> (T_PROGRAM, T_LIBRARY, "TARGET");
  fprintf (out.Makefile, "%s_%s =", canonical (id (target.name ()), current_symtype).c_str (), ldadd (current_symtype));
  symbol_visitor::visit (n);
  fprintf (out.Makefile, "\n\n");
}

static void
tabbed (FILE *out, std::string const &s)
{
  fprintf (out, "\t\\\n\t%s", s.c_str ());
}

void
emit_link::visit (token &n)
{
  if (n.tok == TK_INT_LIB)
    tabbed (out.Makefile, "lib" + n.string + ".la");
  else if (n.tok == TK_EXT_LIB)
    tabbed (out.Makefile, n.string);
}
