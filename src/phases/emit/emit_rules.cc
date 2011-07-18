#include "phase.h"

#include "colours.h"
#include "foreach.h"

#include <stdexcept>

struct emit_rules
  : visitor
{
  virtual void visit (t_variable &n);
  virtual void visit (t_rule_line &n);
  virtual void visit (t_rule &n);

  virtual void visit (token &n);

  bool in_rule;

  emit_rules (annotation_map &annots)
    : in_rule (false)
  {
  }
};

static phase<emit_rules> thisphase ("emit_rules", noauto);


void
emit_rules::visit (t_variable &n)
{
  phases::run ("print", &n);
}

void
emit_rules::visit (t_rule_line &n)
{
  printf ("\t");
  visitor::visit (n);
  printf ("\n");
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
      printf (": ");
      resume (n.prereq ()) && printf (" ");
      printf ("\n");
      resume (n.code ());
      printf ("\n");
      in_rule = false;
    }
}


void
emit_rules::visit (token &n)
{
  if (in_rule)
    printf ("%s", n.string.c_str ());
}
