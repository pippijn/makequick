#include "phase.h"

#include "util/foreach.h"

/** \brief Concatenate all non-wildcard path parts in filenames.
 */
struct concat_sources
  : visitor
{
  void visit (t_filename &n);

  concat_sources (annotation_map &annots)
  {
  }
};

static phase<concat_sources> thisphase ("concat_sources", "expand_vars");


static bool
is_multi (node_ptr const &n)
{
  if (token const *t = n->is<token> ())
    return t->tok == TK_FN_LBRACE
        || t->tok == TK_FN_RBRACE;
  return false;
}

static bool
can_concat (node_ptr const &n)
{
  if (token const *t = n->is<token> ())
    switch (t->tok)
      {
      case TK_FN_LBRACE:
      case TK_FN_PERCENT:
      case TK_FN_PERPERCENT:
      case TK_FN_QMARK:
      case TK_FN_RBRACE:
      case TK_FN_STAR:
      case TK_FN_STARSTAR:
        return false;
      default:
        return true;
      }
  return false;
}

static void
commit (std::string &fn, node_vec &list, location &loc)
{
  if (!fn.empty ())
    list.push_back (new token (loc, TK_FILENAME, fn));
  fn.clear ();

  loc.first_line = loc.last_line;
  loc.first_column = loc.last_column;
}

void
concat_sources::visit (t_filename &n)
{
  node_vec list;
  swap (list, n.list);

  location loc = n.loc;
  std::string fn;
  bool in_multi = false;
  foreach (node_ptr const &p, list)
    {
      loc.last_line = p->loc.last_line;
      loc.last_column = p->loc.last_column;

      if (is_multi (p))
        in_multi = !in_multi;

      if (in_multi || !can_concat (p))
        {
          commit (fn, n.list, loc);
          n.list.push_back (p);
        }
      else
        fn += p->as<token> ().string;
    }
  commit (fn, n.list, loc);
}
