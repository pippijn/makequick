#include "node.h"

#include "annotations/symbol_table.h"

struct symbol_visitor
  : nodes::visitor
{
  typedef annotations::symbol_table symbol_table;

  void visit (nodes::t_target_definition &n);
  void visit (nodes::t_program &n);
  void visit (nodes::t_library &n);
  void visit (nodes::t_template &n);
  void visit (nodes::t_toplevel_declarations &n);

  symbol_table &symtab;

  enum visit_state
  {
    S_NONE,
    S_PROGRAM,
    S_LIBRARY,
    S_TEMPLATE,
  } state;

  symbol_visitor (symbol_table &symtab)
    : symtab (symtab)
    , state (S_NONE)
  {
  }
};
