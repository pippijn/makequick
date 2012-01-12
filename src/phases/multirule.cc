#include "phase.h"

#include "foreach.h"
#include "util/make_filename.h"

#include <algorithm>
#include <typeinfo>

struct multirule
  : visitor
{
  void visit (t_filename &n);
  void visit (t_rule &n);
  void visit (t_rules &n);
  void visit (t_target_members &n);

  void visit_container (generic_node &n);

  enum rule_state
  {
    S_NONE,
    S_TARGET,
    S_PREREQ
  };

  std::vector<token_vec> instances;
  std::vector<t_rule_ptr> rules;
  rule_state state;

  multirule (annotation_map &annots)
    : state (S_NONE)
  {
  }
};

static phase<multirule> thisphase ("multirule", "resolve_vars");


static bool lbrace (token_ptr const &t) { return t->tok == TK_FN_LBRACE; }
static bool rbrace (token_ptr const &t) { return t->tok == TK_FN_RBRACE; }

void
multirule::visit (t_filename &n)
{
  if (state == S_TARGET)
    {
      std::vector<token_vec> instances;
      try {
        token_vec multifile (n.list.size ());
        transform (n.list.begin (), n.list.end (), multifile.begin (), as<token>);
        instances.push_back (multifile);
      } catch (std::bad_cast const &e) {
        phases::run ("xml", &n);
        throw;
      }

      bool more = true;
      while (more)
        {
          std::vector<token_vec> new_instances;
          new_instances.reserve (instances.size () * 2); // TODO: calculate actual number
          foreach (token_vec const &multifile, instances)
            {
              token_vec::const_iterator lb = find_if (multifile.begin (), multifile.end (), lbrace);
              if (lb == multifile.end ())
                {
                  more = false;
                  continue;
                }
              token_vec::const_iterator rb = find_if (lb, multifile.end (), rbrace);
              assert (rb != multifile.end ());
              assert (std::distance (lb, rb) > 2);

              token_vec const instance (multifile.begin (), lb);

              token_vec const variations (++lb, rb);
              ++rb;

              foreach (token_ptr const &p, variations)
                {
                  new_instances.push_back (instance);
                  token_vec &instance = new_instances.back ();
                  instance.push_back (p);
                  instance.insert (instance.end (), rb, multifile.end ());
                }
            }
          if (more)
            swap (new_instances, instances);
        }
      this->instances.insert (this->instances.end (), instances.begin (), instances.end ());
    }
}

void
multirule::visit (t_rule &n)
{
  state = S_TARGET;
  n.target ()->accept (*this);
  state = S_PREREQ;
  n.prereq ()->accept (*this);
  state = S_NONE;

  token_vec instance0 = instances.back ();
  instances.pop_back ();

  t_filenames &target = n.target ()->as<t_filenames> ();
  location const loc = target[0]->loc;
  t_filename_ptr fn = make_filename (loc, instance0);

  target.list.clear ();
  target.add (fn);

  foreach (token_vec const &instance, instances)
    {
      t_rule_ptr rule = new t_rule
        ( n.loc
        , (new t_filenames (loc))->add (make_filename (loc, instance))
        , &target
        , 0);
      rules.push_back (rule);
    }

  instances.clear ();
}

void
multirule::visit (t_rules &n)
{
  visit_container (n);
}

void
multirule::visit (t_target_members &n)
{
  visit_container (n);
}

void
multirule::visit_container (generic_node &n)
{
  foreach (node_ptr const &p, n.list)
    p->accept (*this);
  n.list.insert (n.list.end (), rules.begin (), rules.end ());
  rules.clear ();
}
