#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
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

//static phase<flatten_filenames> thisphase ("flatten_filenames", "inference", "resolve_sources");
static phase<flatten_filenames> thisphase ("flatten_filenames", "resolve_vars");


void
flatten_filenames::visit (t_filename &n)
    // TODO: after inference and source filename resolution, this code works
#if 0
{
  node_vec const &body = unlink_all (n.list);

  std::string code;
  location loc = location::invalid;
  foreach (node_ptr const &p, body)
    {
      if (token_ptr tok = p->is<token> ())
        {
          assert (tok->tok == TK_FILENAME);
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
          n.add (new token (loc, TK_FILENAME, move (code)));
          loc = location::invalid;
          n.add (p);
        }
    }
  if (loc != location::invalid)
    {
      assert (!code.empty ());
      n.add (new token (loc, TK_FILENAME, move (code)));
    }
}
#else
{
  node_vec const &body = unlink_all (n.list);

  std::string code;
  location loc = location::invalid;
  foreach (node_ptr const &p, body)
    {
      token_ptr tok = p->is<token> ();
      if (tok && tok->tok == TK_FILENAME)
        {
          if (loc == location::invalid)
            {
              assert (code.empty ());
              loc = tok->loc;
            }
          code += tok->string;
        }
      else
        {
          if (!code.empty ())
            {
              assert (loc != location::invalid);
              n.add (new token (loc, TK_FILENAME, move (code)));
              loc = location::invalid;
            }
          n.add (p);
        }
    }
  if (loc != location::invalid)
    {
      assert (!code.empty ());
      n.add (new token (loc, TK_FILENAME, move (code)));
    }
}
#endif
