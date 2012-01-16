#include "phase.h"

#include "colours.h"
#include "foreach.h"
#include "util/symbol_visitor.h"

#include <boost/filesystem/path.hpp>

struct resolve_tools
  : symbol_visitor
{
  virtual void visit (t_filename &n);
  virtual void visit (t_rule &n);

  enum parse_state
  {
    S_NONE,
    S_RULE
  };

  parse_state state;

  resolve_tools (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , state (S_NONE)
  {
  }

#if 0
  ~resolve_tools ()
  {
    if (!std::uncaught_exception ())
      throw 0;
  }
#endif
};

static phase<resolve_tools> thisphase ("resolve_tools", "instantiate_rules");

void
resolve_tools::visit (t_filename &n)
{
  if (state != S_RULE)
    return;

  if (n.size () > 1)
    return;
  assert (n.size () == 1);

  token const &t = n[0]->as<token> ();
  std::string const &file = t.string;

  if (generic_node_ptr sym = symtab.lookup (T_PROGRAM, file))
    n[0] = new token (t.loc, t.tok, t.string + "$(EXEEXT)");
}

void
resolve_tools::visit (t_rule &n)
{
  state = S_RULE;
  resume (n.prereq ());
  state = S_NONE;
}
