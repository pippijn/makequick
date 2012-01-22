#include "phase.h"

#include "annotations/output_file.h"
#include "annotations/symbol_table.h"
#include "util/canonical.h"
#include "util/colours.h"
#include "util/extract_string.h"
#include "util/foreach.h"
#include "util/grep.h"
#include "util/symbol_visitor.h"
#include "util/uc.h"

#include <stdexcept>
#include <tr1/unordered_set>

struct emit_headers
  : symbol_visitor
{
  virtual void visit (t_target_definition &n);
  virtual void visit (t_headers &n);

  virtual void visit (token &n);

  bool emit;
  t_if_ptr cond;
  output_file const &out;

  emit_headers (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , emit (false)
    , out (annots.get ("output"))
  {
  }
};

static phase<emit_headers> thisphase ("emit_headers", "emit");


void
emit_headers::visit (t_target_definition &n)
{
  cond = n.cond () ? &n.cond ()->as<t_if> () : NULL;
  symbol_visitor::visit (n);
  cond = NULL;
}


void
emit_headers::visit (t_headers &n)
{
  if (cond)
    fprintf (out.Makefile, "if %s\n", id (cond->cond ()).c_str ());
  if (n.cond ())
    fprintf (out.Makefile, "if %s\n", id (n.cond ()->as<t_if> ().cond ()).c_str ());
  emit = true;
  fprintf (out.Makefile, "%s_HEADERS +=", id (n.dest ()->as<t_destination> ().dir ()).c_str ());
  resume (n.sources ());
  fprintf (out.Makefile, "\n");
  emit = false;
  if (n.cond ())
    fprintf (out.Makefile, "endif\n");
  if (cond)
    fprintf (out.Makefile, "endif\n");
}

void
emit_headers::visit (token &n)
{
  if (emit)
    fprintf (out.Makefile, " %s", n.string.c_str ());
}
