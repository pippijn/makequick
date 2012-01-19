#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "util/foreach.h"
#include "util/move.h"
#include "util/unlink.h"

struct flatten_vars
  : visitor
{
  void visit (t_vardecl_body &n);

  error_log &errors;

  flatten_vars (annotation_map &annots)
    : errors (annots.get ("errors"))
  {
  }
};

static phase<flatten_vars> thisphase ("flatten_vars", "resolve_vars");


void
flatten_vars::visit (t_vardecl_body &n)
{
  visitor::visit (n);
  if (t_vardecl_body_ptr parent = n.parent ()->is<t_vardecl_body> ())
    parent->replace (n, n.list);

  node_vec const &body = unlink_all (n.list);

  std::string code;
  location loc = location::invalid;
  foreach (node_ptr const &p, body)
    {
      if (token_ptr tok = p->is<token> ())
        {
          assert (tok->tok == TK_CODE);
          if (loc == location::invalid)
            {
              assert (code.empty ());
              loc = tok->loc;
            }
          code += tok->string;
        }
      else
        {
          assert (loc != location::invalid);
          n.add (new token (loc, TK_CODE, move (code)));
          loc = location::invalid;
          n.add (p);
        }
    }
  if (loc != location::invalid)
    {
      assert (!code.empty ());
      n.add (new token (loc, TK_CODE, move (code)));
    }
}
