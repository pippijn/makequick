#include "phase.h"

#include "annotations/output_file.h"

struct emit_built_sources
  : visitor
{
  virtual void visit (t_built_sources &n);
  virtual void visit (token &n);

  output_file const &out;
  bool emit;

  emit_built_sources (annotation_map &annots)
    : out (annots.get ("output"))
    , emit (false)
  {
  }
};

static phase<emit_built_sources> thisphase ("emit_built_sources", "emit");

void
emit_built_sources::visit (t_built_sources &n)
{
  emit = true;
  fprintf (out.Makefile, "BUILT_SOURCES =");
  visitor::visit (n);
  fprintf (out.Makefile, "\n\n");
  emit = false;
}

void
emit_built_sources::visit (token &n)
{
  if (emit)
    fprintf (out.Makefile, " %s", n.string.c_str ());
}
