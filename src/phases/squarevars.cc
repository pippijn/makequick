#include "phase.h"

#include "util/symbol_visitor.h"

struct squarevars
  : symbol_visitor
{
  virtual void visit (t_squarevar &n);
  virtual void visit (t_rule_line &n);
  virtual void visit (t_rule &n);

  node_vec replacement;

  squarevars (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
  {
  }

#if 0
  ~squarevars ()
  {
    if (!std::uncaught_exception ())
      throw 0;
  }
#endif
};

static phase<squarevars> thisphase ("squarevars", "instantiate_rules", "insert_syms");

static node_ptr
make_var (location const &loc, std::string const &name)
{
  return new t_roundvar (loc,
           new t_variable_content (loc,
             new token (loc,
               TK_IDENTIFIER,
               name),
             0,
             0));
}

static node_ptr
make_space (location const &loc)
{
  return new token (loc, TK_CODE, " ");
}

void
squarevars::visit (t_squarevar &n)
{
  generic_node_ptr TARGET = symtab.lookup (T_VARIABLE, "TARGET");
  assert (TARGET);
  generic_node &target = TARGET->as<generic_node> ();
  std::string const &tname = target[0]->as<token> ().string;
  std::string const &vname = n.name ()->as<token> ().string;

  replacement.push_back (make_var (n.loc, tname + "_" + vname));
  replacement.push_back (make_space (n.loc));
  replacement.push_back (make_var (n.loc, "AM_" + vname));
  replacement.push_back (make_space (n.loc));
  replacement.push_back (make_var (n.loc, vname));
}

void
squarevars::visit (t_rule_line &n)
{
  for (size_t i = 0; i < n.size (); i++)
    {
      node_ptr const &p = n[i];
      resume (p);
      if (!replacement.empty ())
        {
          n.list.erase  (n.list.begin () + i, n.list.begin () + i + 1);
          n.list.insert (n.list.begin () + i, replacement.begin (), replacement.end ());

          replacement.clear ();
        }
    }
}

void
squarevars::visit (t_rule &n)
{
  generic_node const &target = n.target ()->as<generic_node> ();
  if (target.size () != 1)
    return;
  if (target[0]->as<generic_node> ().size () != 1)
    return;
  // only consider instantiated rules
  visitor::visit (n);
}
