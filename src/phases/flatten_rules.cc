#include <algorithm>

#include "phase.h"

#include "util/make_code.h"
#include "util/unlink.h"

struct flatten_rules
  : visitor
{
  void visit (t_filename &n);
  void visit (t_roundvar &n);
  void visit (t_rule_line &n);

  flatten_rules (annotation_map &annots)
  {
  }
};

static phase<flatten_rules> thisphase ("flatten_rules", "squarevars");


static bool
cannot_flatten_filename (node_ptr const &n)
{
  token_ptr tok = n->is<token> ();
  return !tok || tok->tok != TK_FILENAME;
}

static bool
cannot_flatten_rule (node_ptr const &n)
{
  token_ptr tok = n->is<token> ();
  return !tok || tok->tok != TK_CODE;
}

static bool
can_flatten (node_list &n)
{
  if (n.size () > 1)
    return false;

  if (!n.parent ()->is<t_rule_line> ())
    return false;

  return true;
}

void
flatten_rules::visit (t_filename &n)
{
  if (!can_flatten (n))
    return;

  if (find_if (n.list.begin (), n.list.end (), cannot_flatten_filename) != n.list.end ())
    return;

  n.parent ()->replace (n, make_code (id (n[0])));
}

void
flatten_rules::visit (t_roundvar &n)
{
  if (!can_flatten (n))
    return;

  n.parent ()->replace (n, make_code ("$(" + id (n[0]) + ")"));
}

void
flatten_rules::visit (t_rule_line &n)
{
  visitor::visit (n);

  if (find_if (n.list.begin (), n.list.end (), cannot_flatten_rule) != n.list.end ())
    return;

  std::string code;
  foreach (node_ptr const &p, n.list)
    code += id (p);

  unlink_all (n.list);
  n.add (make_code (n.loc, code));
}
