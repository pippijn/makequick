#include "phase.h"

#include "annotations/output_file.h"
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

  virtual void visit (t_sources &n);
  virtual void visit (t_nodist_sources &n);
  virtual void visit (t_extra_dist &n);

  virtual void visit (t_filename &n);
  virtual void visit (t_sources_members &n);
  virtual void visit (t_rule &n);

  virtual void visit (token &n);

  enum source_state
  {
    SS_NONE,
    SS_SOURCES,
    SS_NODIST_SOURCES,
    SS_EXTRA_DIST,
  } sstate;

  bool in_filename;
  annotations::output_file const &out;

  emit_SOURCES (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , sstate (SS_NONE)
    , in_filename (false)
    , out (annots.get ("output"))
  {
  }
};

static phase<emit_SOURCES> thisphase ("emit_SOURCES", noauto);

void
emit_SOURCES::visit (t_sources &n)
{
  local (sstate) = SS_SOURCES;
  visitor::visit (n);
}

void
emit_SOURCES::visit (t_nodist_sources &n)
{
  local (sstate) = SS_NODIST_SOURCES;
  visitor::visit (n);
}

void
emit_SOURCES::visit (t_extra_dist &n)
{
  local (sstate) = SS_EXTRA_DIST;
  visitor::visit (n);
}


void
emit_SOURCES::visit (t_filename &n)
{
  local (in_filename) = true;
  visitor::visit (n);
}

void
emit_SOURCES::visit (t_sources_members &n)
{
  if (sstate != SS_EXTRA_DIST)
    {
      generic_node_ptr TARGET = symtab.lookup (T_VARIABLE, "TARGET");
      assert (TARGET);
      generic_node &name = TARGET->as<generic_node> ();
      fprintf (out.Makefile, "%s%s_SOURCES =",
               sstate == SS_NODIST_SOURCES ? "nodist_" : "",
               canonical (name[0]->as<token> ().string, state).c_str ());
    }
  else
    {
      fprintf (out.Makefile, "EXTRA_DIST +=");
    }
  visitor::visit (n);
  fprintf (out.Makefile, "\n\n");
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
    fprintf (out.Makefile, "\t\\\n\t%s", n.string.c_str ());
}
