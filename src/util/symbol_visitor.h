#include "node.h"

struct symbol_table;

struct symbol_visitor
  : nodes::visitor
{
  void visit (nodes::generic_node &n);

  symbol_table &symtab;

  nodes::node_type current_symtype;

  symbol_visitor (symbol_table &symtab);

  virtual bool override (nodes::generic_node &n) { return false; }
};
