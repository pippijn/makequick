#include "phase.h"

#include "annotations/symbol_table.h"
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

static phase<default_prereq> thisphase ("default_prereq", "inheritance");


void
default_prereq::visit (t_filenames &n)
{
  if (state == S_RULE)
    {
      generic_node_ptr target = symtab.lookup (T_VARIABLE, "THIS");
      if (!target)
        return;

      std::string const &name = id (target->as<t_vardecl_body> ()[0]);

      size_t from = strlen ("$(builddir)/");
      size_t to = name.size () - from - strlen ("$(EXEEXT)");
      n.add (make_filename (name.substr (from, to)));
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
