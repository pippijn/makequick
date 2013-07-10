#include "phase.h"

#include "annotations/symbol_table.h"
#include "util/ancestor.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/grep.h"
#include "util/symbol_visitor.h"
#include "util/uc.h"

struct resolve_flagsref
  : symbol_visitor
{
  virtual void visit (t_flag_import &n);

  resolve_flagsref (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
  {
  }

#if 0
  ~resolve_flagsref ()
  {
    if (!std::uncaught_exception ())
      throw 0;
  }
#endif
};

static phase<resolve_flagsref> thisphase ("resolve_flagsref", "inheritance", "insert_global_syms");


void
resolve_flagsref::visit (t_flag_import &n)
{
  std::string const &tool = id (n.tool ());
  if (tool == id (ancestor<t_tool_flags> (n)->keyword ()) && !n.target ())
    {
      n.parent ()->replace (n, new token (n.loc, TK_FLAG, "$(AM_" + uc (tool) + ")"));
      return;
    }

  t_target_definition &target = symtab.lookup (T_PROGRAM, T_LIBRARY, id (n.target ()))->as<t_target_definition> ();

  foreach (t_tool_flags &flags, grep<t_tool_flags> (target.body ()))
    if (id (flags.keyword ()) == tool)
      ancestor<t_target_members> (n)->add (flags.clone ());

  ancestor<t_flags> (n)->unlink ();
}
