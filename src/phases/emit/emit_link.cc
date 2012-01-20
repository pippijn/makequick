#include "phase.h"

#include "annotations/output_file.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/symbol_visitor.h"

#include <stdexcept>

struct emit_link
  : symbol_visitor
{
  static std::string canonical (std::string const &name, visit_state state)
  {
    if (state == S_PROGRAM)
      return name + "_LDADD";
    if (state == S_LIBRARY)
      return "lib" + name + "_la_LIBADD";
    throw std::invalid_argument ("invalid state in target " + C::quoted (name));
  }

  virtual void visit (t_link_body &n);

  virtual void visit (token &n);

  output_file const &out;

  emit_link (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , out (annots.get ("output"))
  {
  }
};

static phase<emit_link> thisphase ("emit_link", noauto);


void
emit_link::visit (t_link_body &n)
{
  generic_node_ptr TARGET = symtab.lookup (T_VARIABLE, "TARGET");
  assert (TARGET);
  generic_node &name = TARGET->as<generic_node> ();
  fprintf (out.Makefile, "%s =", canonical (id (name[0]), state).c_str ());
  visitor::visit (n);
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
