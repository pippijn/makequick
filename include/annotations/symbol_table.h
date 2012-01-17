#pragma once

#include "annotation.h"

#include "node_ptr.h"

#include <map>


enum symbol_type
{
  T_VARIABLE,
  T_PROGRAM,
  T_LIBRARY,
  T_TEMPLATE
};

struct symbol_table
  : annotation
{
  typedef nodes::generic_node_ptr generic_node_ptr;

  symbol_table ();

  void enter_scope (nodes::generic_node_ptr const &scope);
  nodes::generic_node_ptr leave_scope ();

  bool insert (symbol_type type, std::string const &name, nodes::generic_node_ptr const &id);
  bool insert_global (symbol_type type, std::string const &name, nodes::generic_node_ptr const &id);
  generic_node_ptr lookup (symbol_type type, std::string const &name) const;

  void clear () { assert (stack.empty ()); scopes.clear (); }
  void print () const;
  void print_stack () const;

private:
  typedef std::map<std::string, generic_node_ptr> node_map;
  typedef std::vector<node_map> type_map;
  typedef std::map<generic_node_ptr, type_map> scope_map;

  std::vector<type_map *> stack;
  scope_map scopes;
  nodes::generic_node_ptr current_scope;

  static symbol_table::node_map &get_scope (symbol_table::type_map &map, symbol_type type);
  static bool insert (node_map &scope, std::string const &name, nodes::generic_node_ptr const &id);

  static void print (symbol_table::node_map const &type, char const *type_name);
  static void print (symbol_table::scope_map::value_type const &scope);
};
