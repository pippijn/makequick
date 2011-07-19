#include "phase.h"

#include "colours.h"
#include "foreach.h"
#include "util/symbol_visitor.h"

#include <stdexcept>

struct emit_SOURCES
  : symbol_visitor
{
  static std::string canonical (std::string const &name, visit_state state)
  {
    if (state == S_PROGRAM)
      return name;
    if (state == S_LIBRARY)
      return "lib" + name + "_la";
    throw std::invalid_argument ("invalid state in target " + C::quoted (name));
  }

  virtual void visit (t_filename &n);
  virtual void visit (t_sources_members &n);
  virtual void visit (t_rule &n);

  virtual void visit (token &n);

  bool in_filename;

  emit_SOURCES (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , in_filename (false)
  {
  }
};

static phase<emit_SOURCES> thisphase ("emit_SOURCES", noauto);


void
emit_SOURCES::visit (t_filename &n)
{
  local (in_filename) = true;
  visitor::visit (n);
}

void
emit_SOURCES::visit (t_sources_members &n)
{
  generic_node_ptr TARGET = symtab.lookup (T_VARIABLE, "TARGET");
  assert (TARGET);
  generic_node &name = TARGET->as<generic_node> ();
  printf ("%s_SOURCES =", canonical (name[0]->as<token> ().string.c_str (), state).c_str ());
  visitor::visit (n);
  printf ("\n\n");
}

void
emit_SOURCES::visit (t_rule &n)
{
  // don't go into the rule
}


void
emit_SOURCES::visit (token &n)
{
  if (in_filename)
    printf ("\t\\\n\t%s", n.string.c_str ());
}
