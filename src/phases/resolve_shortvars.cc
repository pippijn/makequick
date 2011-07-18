#include "phase.h"

#include "annotations/error_log.h"
#include "colours.h"
#include "foreach.h"

using annotations::error_log;

struct resolve_shortvars
  : visitor
{
  virtual void visit (t_variable &n);

  virtual void visit (t_rule_line &n);
  virtual void visit (t_rule &n);

  error_log &errors;

  enum parse_state
  {
    S_NONE,
    S_FILENAME,
    S_MULTIFILE
  };

  parse_state state;
  node_ptr replacement;
  t_filename_ptr target;
  t_filenames_ptr prereq;

  resolve_shortvars (annotation_map &annots)
    : errors (annots.get ("errors"))
    , state (S_NONE)
  {
  }

#if 0
  ~resolve_shortvars ()
  {
    if (!std::uncaught_exception ())
      exit (0);
  }
#endif
};

static phase<resolve_shortvars> thisphase ("resolve_shortvars", "instantiate_rules");


void
resolve_shortvars::visit (t_variable &n)
{
  if (token *tok = n.content ()->is<token> ())
    {
      if (tok->tok == TK_SHORTVAR)
        {
          assert (target);
          assert (prereq);

          // uninstantiated rule:
          if (target->size () > 1)
            return;

          switch (tok->string[0])
            {
            case '@':
              replacement = target;
              break;
            case '<':
              assert (prereq->size () >= 1);
              replacement = prereq->list[0];
              break;
            }
        }
    }
}


void
resolve_shortvars::visit (t_rule_line &n)
{
  foreach (node_ptr &p, n.list)
    {
      resume (p);
      if (replacement)
        {
          swap (p, replacement);
          replacement = 0;
        }
    }
}

void
resolve_shortvars::visit (t_rule &n)
{
  target = &n.target ()->as<t_filenames> ()[0]->as<t_filename> ();
  prereq = &n.prereq ()->as<t_filenames> ();

  visitor::visit (n);

  prereq = 0;
  target = 0;
}
