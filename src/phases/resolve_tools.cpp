#include "phase.h"

#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/symbol_visitor.h"

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

  std::string const &file = id (n[0]);

  if (generic_node_ptr sym = symtab.lookup (T_PROGRAM, file))
    n.replace (0, new token (n.loc, TK_FILENAME, file + "$(EXEEXT)"));
}

void
resolve_tools::visit (t_rule &n)
{
  state = S_RULE;
  resume (n.prereq ());
  state = S_NONE;
}
