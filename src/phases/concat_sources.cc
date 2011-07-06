/**
 * concat_sources:
 *
 * This phase concatenates all non-wildcard path parts in filenames.
 */
#include "phase.h"

#include "algorithm/grep.h"
#include "annotations/error_log.h"
#include "annotations/file_list.h"
#include "colours.h"
#include "foreach.h"
#include "util/create_file_list.h"

#include <numeric>

#include <boost/regex.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace
{
  struct concat_sources
    : visitor
  {
    void visit (t_filename &n);

    concat_sources (annotation_map &annots)
    {
    }
  };

  static phase<concat_sources> thisphase ("concat_sources", "resolve_vars");
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

static std::string &
concat_token (std::string &p1, node_ptr const &p2)
{
  return p1 += p2->as<token> ().string;
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
  foreach (node_ptr const &p, list)
    {
      loc.last_line = p->loc.last_line;
      loc.last_column = p->loc.last_column;
      if (!can_concat (p))
        {
          commit (fn, n.list, loc);
          n.list.push_back (p);
        }
      else
        fn += p->as<token> ().string;
    }
  commit (fn, n.list, loc);
}
