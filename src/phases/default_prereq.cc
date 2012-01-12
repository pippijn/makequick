#include "phase.h"

#include "foreach.h"
#include "util/make_filename.h"
#include "util/symbol_visitor.h"

struct default_prereq
  : symbol_visitor
{
  virtual void visit (t_filenames &n);
  virtual void visit (t_rule &n);
  virtual void visit (t_library &n);

  enum parse_state
  {
    S_NONE,
    S_RULE
  };

  parse_state state;

  default_prereq (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
  {
  }
};

static phase<default_prereq> thisphase ("default_prereq", "insert_syms");


void
default_prereq::visit (t_filenames &n)
{
  if (state == S_RULE)
    {
      generic_node_ptr TARGET = symtab.lookup (T_VARIABLE, "TARGET");
      if (!TARGET)
        return;

      generic_node &target = TARGET->as<generic_node> ();
      std::string const &name = target[0]->as<token> ().string;

      n.add (make_filename (location::generated, name));
    }
}

void
default_prereq::visit (t_rule &n)
{
  state = S_RULE;
  resume (n.prereq ());
  state = S_NONE;
}

void
default_prereq::visit (t_library &n)
{
  // ignore libraries
  // TODO: error if rules exist in libraries?
}
