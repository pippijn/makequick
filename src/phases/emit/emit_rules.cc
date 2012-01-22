#include "phase.h"

#include "annotations/output_file.h"
#include "util/colours.h"
#include "util/foreach.h"

#include <stdexcept>

struct emit_rules
  : visitor
{
  virtual void visit (t_rule_line &n);
  virtual void visit (t_rule &n);

  virtual void visit (token &n);

  bool in_rule;
  bool in_first_line;
  bool in_prereq;
  output_file const &out;

  emit_rules (annotation_map &annots)
    : in_rule (false)
    , in_first_line (false)
    , in_prereq (false)
    , out (annots.get ("output"))
  {
  }
};

static phase<emit_rules> thisphase ("emit_rules", "emit");


static bool
has_code (t_rule_line &n)
{
  return n.size () && !id (n[0]).empty ();
}

void
emit_rules::visit (t_rule_line &n)
{
  bool empty = !has_code (n);
  if (in_first_line)
    // TODO: do this somewhere else
    if (empty)
      fprintf (out.Makefile, "\t@:\n");
    else
      fprintf (out.Makefile, "\t@$(MKDIR_P) `dirname $@`\n");

  if (empty)
    return;

  fprintf (out.Makefile, "\t");
  visitor::visit (n);
  fprintf (out.Makefile, "\n");

  in_first_line = false;
}

void
emit_rules::visit (t_rule &n)
{
  // size == 1 => instantiated rule
  // code != NULL => has rule body (no import)

  t_filenames const &targets = n.target ()->as<t_filenames> ();
  t_filename  const &target_file = targets[0]->as<t_filename> ();

  if (target_file.size () == 1 && n.code ())
    {
      in_rule = true;
      resume (n.target ());
      fprintf (out.Makefile, ":");
      {
        in_prereq = true;
        resume (n.prereq ());
        in_prereq = false;
      }
      fprintf (out.Makefile, "\n");
      in_first_line = true;
      resume (n.code ());
      fprintf (out.Makefile, "\n");
      in_rule = false;
    }
}


void
emit_rules::visit (token &n)
{
  if (in_rule)
    {
      if (in_prereq)
        fprintf (out.Makefile, " ");
      fprintf (out.Makefile, "%s", n.string.c_str ());
    }
}
