#include "phase.h"

#include "foreach.h"

struct concat_vars
  : visitor
{
  void visit (t_vardecl_body &n);

  concat_vars (annotation_map &annots)
  {
  }
};

static phase<concat_vars> thisphase ("concat_vars");


static bool
can_concat (node_ptr const &n)
{
  return n->is<token> ();
}

static void
commit (std::string &fn, node_vec &list, location &loc)
{
  if (!fn.empty ())
    {
      list.push_back (new token (loc, TK_CODE, fn));
      fn.clear ();
    }
}

void
concat_vars::visit (t_vardecl_body &n)
{
  node_vec list;
  swap (list, n.list);

  location loc = n.loc;
  std::string fn;
  foreach (node_ptr const &p, list)
    {
      loc.last_line = p->loc.last_line;
      loc.last_column = p->loc.last_column;
      if (!can_concat (p))
        {
          commit (fn, n.list, loc);
          loc.first_line = loc.last_line;
          loc.first_column = loc.last_column;
          n.list.push_back (p);
        }
      else
        {
          std::string const &s = p->as<token> ().string;
          if (s[0] != ' ')
            fn += " ";
          fn += s;
        }
    }
  commit (fn, n.list, loc);
}
