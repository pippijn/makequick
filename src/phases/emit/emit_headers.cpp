#include "phase.h"

#include "annotations/output_file.h"
#include "annotations/symbol_table.h"
#include "util/canonical.h"
#include "util/colours.h"
#include "util/extract_string.h"
#include "util/foreach.h"
#include "util/grep.h"
#include "util/plus_writer.h"
#include "util/symbol_visitor.h"
#include "util/uc.h"

#include <stdexcept>
#include <tr1/unordered_set>

struct emit_headers
  : symbol_visitor
  , plus_writer
{
  virtual void visit (t_target_definition &n);
  virtual void visit (t_headers &n);
  virtual void visit (t_data &n);
  virtual void visit (t_scripts &n);

  template<typename T>
  void print_target (T &n, char const *kind);

  virtual void visit (token &n);

  bool emit;
  t_if_ptr target_cond;
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
  target_cond = n.cond () ? &n.cond ()->as<t_if> () : NULL;
  symbol_visitor::visit (n);
  target_cond = NULL;
}

template<typename T>
void
emit_headers::print_target (T &n, char const *kind)
{
  if (target_cond)
    fprintf (out.Makefile, "if %s\n", id (target_cond->cond ()).c_str ());
  if (n.cond ())
    fprintf (out.Makefile, "if %s\n", id (n.cond ()->as<t_if> ().cond ()).c_str ());
  emit = true;
  plus (out.Makefile, id (n.dest ()->as<t_destination> ().dir ()), kind);
  resume (n.sources ());
  fprintf (out.Makefile, "\n");
  emit = false;
  if (n.cond ())
    fprintf (out.Makefile, "endif\n");
  if (target_cond)
    fprintf (out.Makefile, "endif\n");
}

void
emit_headers::visit (t_headers &n)
{
  print_target (n, "HEADERS");
}

void
emit_headers::visit (t_data &n)
{
  print_target (n, "DATA");
}

void
emit_headers::visit (t_scripts &n)
{
  print_target (n, "SCRIPTS");
}

void
emit_headers::visit (token &n)
{
  if (emit)
    fprintf (out.Makefile, " %s", n.string.c_str ());
}
