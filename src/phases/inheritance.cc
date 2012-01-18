#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "colours.h"
#include "foreach.h"
#include "util/symbol_visitor.h"
#include "util/unique_visitor.h"

#include <stdexcept>

struct inheritance
  : symbol_visitor
  , unique_visitor
{

  generic_node_ptr lookup (std::string const &name) const
  {
    return symtab.lookup (T_TEMPLATE, name);
  }

  void visit (t_target_definition &n);

  error_log &errors;

  inheritance (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , errors (annots.get ("errors"))
  {
  }
};

static phase<inheritance> thisphase ("inheritance", "insert_target_syms");


void
inheritance::visit (t_target_definition &n)
{
  if (done (n))
    return;
  mark (n);

  // this target doesn't inherit anything
  if (!n.derive ())
    return;

  std::string const &name = n.derive ()->as<t_inheritance> ().base ()->as<token> ().string;

  // find referenced template
  generic_node_ptr base = lookup (name);

  if (!base)
    {
      errors.add<semantic_error> (n.derive (), "no such template: " + C::quoted (name));
      return;
    }

  // visit inherited templates, first
  base->accept (*this);

  // now copy all information from the base definition
  t_target_definition &b = base->as<t_target_definition> ();

  if (b.cond ())
    {
      assert (!n.cond ());
      n.cond (b.cond ()->clone ());
    }
  if (b.dest ())
    {
      assert (!n.dest ());
      n.dest (b.dest ()->clone ());
    }
  t_target_members &body = n.body ()->as<t_target_members> ();
  foreach (node_ptr const &p, b.body ()->as<t_target_members> ().list)
    body.add (p->clone ());
}
