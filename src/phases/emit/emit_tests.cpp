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
#include <unordered_set>

struct emit_test
  : symbol_visitor
  , plus_writer
{
  virtual void visit (t_log_compilers &n);
  virtual void visit (t_target_definition &n);
  virtual void visit (t_test &n);

  virtual void visit (token &n);

  bool emit;
  t_if_ptr cond;
  output_file const &out;

  emit_test (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , emit (false)
    , out (annots.get ("output"))
  {
    plus (out.Makefile, "", "TESTS");
    fprintf (out.Makefile, "# added to, later\n");
    plus (out.Makefile, "XFAIL", "TESTS");
    fprintf (out.Makefile, "# added to, later\n");
  }
};

static phase<emit_test> thisphase ("emit_test", "emit");


void
emit_test::visit (t_log_compilers &n)
{
  fprintf (out.Makefile, "TEST_EXTENSIONS =");
  foreach (node_ptr const &p, n.list)
    {
      t_log_compiler &rule = p->as<t_log_compiler> ();
      fprintf (out.Makefile, " %s", id (rule.ext ()->as<t_filenames> ()[0]->as<t_filename> ()[0]).c_str ());
    }
  fprintf (out.Makefile, "\n");

  foreach (t_log_compiler &rule, grep<t_log_compiler> (n))
    fprintf (out.Makefile, "%s_LOG_COMPILER = %s\n",
             uc (id (rule.ext ()->as<t_filenames> ()[0]->as<t_filename> ()[0])).c_str () + 1,
             extract_string (*rule.rule ()).c_str ());
}

void
emit_test::visit (t_target_definition &n)
{
  cond = n.cond () ? &n.cond ()->as<t_if> () : NULL;
  symbol_visitor::visit (n);
  cond = NULL;
}


static std::string
type_opt (node_ptr const &p)
{
  return p ? uc (id (p)) : std::string ();
}

void
emit_test::visit (t_test &n)
{
  if (cond)
    fprintf (out.Makefile, "if %s\n", id (cond->cond ()).c_str ());
  if (n.cond ())
    fprintf (out.Makefile, "if %s\n", id (n.cond ()->as<t_if> ().cond ()).c_str ());
  emit = true;
  plus (out.Makefile, type_opt (n.type ()), "TESTS");
  resume (n.sources ());
  fprintf (out.Makefile, "\n");
  emit = false;
  if (n.cond ())
    fprintf (out.Makefile, "endif\n");
  if (cond)
    fprintf (out.Makefile, "endif\n");
}

void
emit_test::visit (token &n)
{
  if (emit)
    fprintf (out.Makefile, " %s", n.string.c_str ());
}
