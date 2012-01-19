#include <algorithm>

#include "phase.h"

#include "annotations/symbol_table.h"
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

static phase<squarevars> thisphase ("squarevars", "instantiate_rules");

static node_ptr
make_var (location const &loc, std::string const &name)
{
  return new t_roundvar (loc,
           new token (loc,
             TK_IDENTIFIER,
             name));
}

static node_ptr
make_space (location const &loc)
{
  return new token (loc, TK_CODE, " ");
}

static char
normalise (char c)
{
  if (isalnum (c))
    return c;
  return '_';
}

void
squarevars::visit (t_squarevar &n)
{
  symtab.print_stack ();
  symbol_type type;
  generic_node_ptr TARGET = symtab.lookup (type = T_PROGRAM, "TARGET");
  if (!TARGET)     TARGET = symtab.lookup (type = T_LIBRARY, "TARGET");

  if (!TARGET)
    return;
  puts ("YEP");

  t_target_definition &target = TARGET->as<t_target_definition> ();
  std::string const &tname = target.name ()->as<token> ().string;
  std::string const &vname = n.name ()->as<token> ().string;

  replacement.push_back (make_var (n.loc, "AM_" + vname));
  replacement.push_back (make_space (n.loc));
  replacement.push_back (make_var (n.loc, vname));
  replacement.push_back (make_space (n.loc));

  std::string name = tname;
  if (type == T_LIBRARY)
    name = "lib" + name + "_la";
  name += "_";
  name += vname;

  transform (name.begin (), name.end (), name.begin (), normalise);

  replacement.push_back (make_var (n.loc, name));
}

void
squarevars::visit (t_rule_line &n)
{
  for (size_t i = 0; i < n.size (); i++)
    {
      assert (replacement.empty ());
      node_ptr const &p = n[i];
      resume (p);
      if (!replacement.empty ())
        {
          n.replace (*p, replacement);
          replacement.clear ();
        }
    }
}

void
squarevars::visit (t_rule &n)
{
  generic_node const &target = n.target ()->as<generic_node> ();
  // only consider instantiated rules
  {
    if (target.size () != 1)
      return;
    if (target[0]->as<generic_node> ().size () != 1)
      return;
  }
  visitor::visit (n);
}
