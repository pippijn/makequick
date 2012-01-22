#include "phase.h"

#include "util/foreach.h"
#include "util/make_filename.h"

#include <algorithm>
#include <typeinfo>

struct multirule
  : visitor
{
  void visit (t_filename &n);
  void visit (t_rule &n);

  enum rule_state
  {
    S_NONE,
    S_TARGET
  };

  rule_state state;

  multirule (annotation_map &annots)
    : state (S_NONE)
  {
  }
};

static phase<multirule> thisphase ("multirule", "expand_vars");


template<typename T>
static boost::intrusive_ptr<T>
clone_as (node_ptr const &p)
{
  return &p->clone ()->as<T> ();
}

static int
maybe_tok (node_ptr const &n)
{
  token *tok = n->is<token> ();
  if (!tok)
    return -1;
  return tok->tok;
}

static bool lbrace (node_ptr const &n) { return maybe_tok (n) == TK_FN_LBRACE; }
static bool rbrace (node_ptr const &n) { return maybe_tok (n) == TK_FN_RBRACE; }

void
multirule::visit (t_filename &n)
{
  if (state != S_TARGET)
    return;

  node_vec const &multifile = n.list;

  node_vec::const_iterator const lb = find_if (multifile.begin (), multifile.end (), lbrace);
  node_vec::const_iterator const rb = find_if (lb, multifile.end (), rbrace);

  if (lb == multifile.end ())
    return;

  assert (rb != multifile.end ());
  assert (distance (lb, rb) > 2);

  node_vec::const_iterator option = lb + 1;

  t_filename_ptr common_prereq;
  {
    node_vec target_file;
    for (node_vec::const_iterator copy = multifile.begin (); copy != lb; ++copy)
      target_file.push_back ((*copy)->clone ());
    target_file.push_back ((*option)->clone ());
    for (node_vec::const_iterator copy = rb + 1; copy != multifile.end (); ++copy)
      target_file.push_back ((*copy)->clone ());

    common_prereq = make_filename (n.loc, target_file);
  }
  ++option;

  t_rule &origin = n.parent () // filenames
                   ->parent () // rule
                   ->as<t_rule> ();

  node_list *container = origin.parent (); // container of rule

  for (node_vec::const_iterator it = option; it != rb; ++it)
    {
      node_vec instance;

      for (node_vec::const_iterator copy = multifile.begin (); copy != lb; ++copy)
        instance.push_back ((*copy)->clone ());
      instance.push_back ((*it)->clone ());
      for (node_vec::const_iterator copy = rb + 1; copy != multifile.end (); ++copy)
        instance.push_back ((*copy)->clone ());

      t_rule_ptr rule = new t_rule
        ( n.loc
        , (new t_filenames (n.loc))->add (make_filename (n.loc, instance))
        , origin.prereq ()->clone ()->as<t_filenames> ().add (common_prereq->clone ())
        , origin.code () ? (new t_rule_lines ())->add (new t_rule_line) : NULL);
      container->add (rule);
    }

  n.parent ()->replace (n, common_prereq);
}

void
multirule::visit (t_rule &n)
{
  state = S_TARGET;
  n.target ()->accept (*this);
  state = S_NONE;
}
