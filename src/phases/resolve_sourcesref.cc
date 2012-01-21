#include "phase.h"

#include "annotations/symbol_table.h"
#include "util/ancestor.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/grep.h"
#include "util/symbol_visitor.h"

struct resolve_sourcesref
  : symbol_visitor
{
  virtual void visit (t_sourcesref &n);

  resolve_sourcesref (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
  {
  }

#if 0
  ~resolve_sourcesref ()
  {
    if (!std::uncaught_exception ())
      throw 0;
  }
#endif
};

static phase<resolve_sourcesref> thisphase ("resolve_sourcesref", "resolve_sources", "resolve_wildcards");


// TODO: honour "if" clause
void
resolve_sourcesref::visit (t_sourcesref &n)
{
  t_target_definition &target = symtab.lookup (T_PROGRAM, T_LIBRARY, id (n.ref ()))->as<t_target_definition> ();

  foreach (t_sources &src, grep<t_sources> (target.body ()))
    ancestor<t_target_members> (n)->add (src.clone ());

  n.unlink ();
}
