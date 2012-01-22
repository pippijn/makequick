#include <algorithm>

#include "phase.h"

#include "annotations/error_log.h"
#include "util/foreach.h"
#include "util/move.h"
#include "util/unlink.h"

struct flatten_filenames
  : visitor
{
  void visit (t_filename &n);

  error_log &errors;

  flatten_filenames (annotation_map &annots)
    : errors (annots.get ("errors"))
  {
  }
};

static phase<flatten_filenames> thisphase ("flatten_filenames", "expand_vars");

static bool
cannot_flatten (node_ptr const &n)
{
  token_ptr tok = n->is<token> ();
  return !tok || tok->tok != TK_FILENAME;
}

void
flatten_filenames::visit (t_filename &n)
{
  if (find_if (n.list.begin (), n.list.end (), cannot_flatten) != n.list.end ())
    return;

  node_vec const &body = unlink_all (n.list);

  std::string code;
  location loc = location::invalid;
  foreach (node_ptr const &p, body)
    {
      token &tok = p->as<token> ();
      if (loc == location::invalid)
        {
          assert (code.empty ());
          loc = tok.loc;
        }
      code += tok.string;
    }
  if (loc != location::invalid)
    {
      assert (!code.empty ());
      n.add (new token (loc, TK_FILENAME, move (code)));
    }
}
