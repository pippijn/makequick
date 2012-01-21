#include "phase.h"

#include "annotations/output_file.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/symbol_visitor.h"

#include <stdexcept>

#include "canonical.h"

struct emit_SOURCES
  : symbol_visitor
{
  virtual void visit (t_sources &n);
  virtual void visit (t_nodist_sources &n);
  virtual void visit (t_extra_dist &n);

  virtual void visit (t_filename &n);
  virtual void visit (t_sources_members &n);
  virtual void visit (t_rule &n);

  virtual void visit (token &n);

  enum state
  {
    S_NONE,
    S_SOURCES,
    S_NODIST_SOURCES,
    S_EXTRA_DIST,
  } state;

  bool in_filename;
  output_file const &out;

  emit_SOURCES (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , state (S_NONE)
    , in_filename (false)
    , out (annots.get ("output"))
  {
  }
};

static phase<emit_SOURCES> thisphase ("emit_SOURCES", "emit");

void
emit_SOURCES::visit (t_sources &n)
{
  local (state) = S_SOURCES;
  symbol_visitor::visit (n);
}

void
emit_SOURCES::visit (t_nodist_sources &n)
{
  local (state) = S_NODIST_SOURCES;
  symbol_visitor::visit (n);
}

void
emit_SOURCES::visit (t_extra_dist &n)
{
  local (state) = S_EXTRA_DIST;
  symbol_visitor::visit (n);
}


void
emit_SOURCES::visit (t_filename &n)
{
  local (in_filename) = true;
  symbol_visitor::visit (n);
}

void
emit_SOURCES::visit (t_sources_members &n)
{
  char const *nodist_opt = "";
  switch (state)
    {
    case S_NODIST_SOURCES:
      nodist_opt = "nodist_";
    case S_SOURCES:
      {
        t_target_definition &target = symtab.lookup<t_target_definition> (T_PROGRAM, T_LIBRARY, "TARGET");
        fprintf (out.Makefile, "%s%s_SOURCES =",
                 nodist_opt, canonical (id (target.name ()), current_symtype).c_str ());
        break;
      }
    case S_EXTRA_DIST:
      fprintf (out.Makefile, "EXTRA_DIST =");
      break;

    default:
      return;
    }
  symbol_visitor::visit (n);
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
  if (in_filename && state != S_NONE)
    fprintf (out.Makefile, "\t\\\n\t%s", n.string.c_str ());
}
