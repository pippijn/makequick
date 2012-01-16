#include "phase.h"

#include "annotations/output_file.h"
#include "colours.h"
#include "foreach.h"
#include "../print.h"

#include <stdexcept>

struct emit_rules
  : visitor
{
  virtual void visit (t_shortvar &n);
  virtual void visit (t_intvar &n);
  virtual void visit (t_roundvar &n);
  virtual void visit (t_squarevar &n);
  virtual void visit (t_rule_line &n);
  virtual void visit (t_rule &n);

  virtual void visit (token &n);

  bool in_rule;
  bool in_prereq;
  output_file const &out;
  print p;

  emit_rules (annotation_map &annots)
    : in_rule (false)
    , in_prereq (false)
    , out (annots.get ("output"))
    , p (out.Makefile)
  {
  }
};

static phase<emit_rules> thisphase ("emit_rules", noauto);


void
emit_rules::visit (t_shortvar &n)
{
  p.visit (n);
}

void
emit_rules::visit (t_intvar &n)
{
  p.visit (n);
}

void
emit_rules::visit (t_squarevar &n)
{
  p.visit (n);
}

void
emit_rules::visit (t_roundvar &n)
{
  p.visit (n);
}

void
emit_rules::visit (t_rule_line &n)
{
  fprintf (out.Makefile, "\t");
  visitor::visit (n);
  fprintf (out.Makefile, "\n");
}

void
emit_rules::visit (t_rule &n)
{
  // size == 1 => instantiated rule
  // code != NULL => has rule body (no import)

  generic_node const &targets = n.target ()->as<generic_node> ();
  generic_node const &target_file = targets[0]->as<generic_node> ();

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
