#include "phase.h"

#include "annotations/output_file.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/grep.h"
#include "util/symbol_visitor.h"
#include "util/uc.h"
#include "util/canonical.h"

#include <stdexcept>
#include <tr1/unordered_set>
#include <tr1/unordered_map>

struct emit_flags
  : symbol_visitor
{
  virtual void visit (t_tool_flags &n);
  virtual void visit (t_roundvar &n);
  virtual void visit (token &n);

  output_file const &out;
  std::tr1::unordered_map<std::string, std::tr1::unordered_set<std::string> > seen;
  bool in_flags;

  emit_flags (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , out (annots.get ("output"))
    , in_flags (false)
  {
  }
};

static phase<emit_flags> thisphase ("emit_flags", "emit");


void
emit_flags::visit (t_tool_flags &n)
{
  generic_node_ptr target = symtab.lookup (T_PROGRAM, T_LIBRARY, "TARGET");

  std::string name = "AM";
  std::string const &keyword = uc (id (n.keyword ()));

  if (target)
    name = canonical (id (target->as<t_target_definition> ().name ()), current_symtype);

  fprintf (out.Makefile, "%s_%s ", name.c_str (), keyword.c_str ());

  if (seen[keyword].find (name) != seen[keyword].end ())
    fprintf (out.Makefile, "+");
  else
    seen[keyword].insert (name);
  fprintf (out.Makefile, "=");

  in_flags = true;
  foreach (t_flag &flag, grep<t_flag> (n.flags ()))
    {
      fprintf (out.Makefile, " ");
      flag.accept (*this);
    }
  in_flags = false;

  fprintf (out.Makefile, "\n\n");
}

void
emit_flags::visit (t_roundvar &n)
{
  if (in_flags)
    {
      fprintf (out.Makefile, "$(");
      visitor::visit (n);
      fprintf (out.Makefile, ")");
    }
}

void
emit_flags::visit (token &n)
{
  if (in_flags)
    fprintf (out.Makefile, "%s", n.string.c_str ());
}
