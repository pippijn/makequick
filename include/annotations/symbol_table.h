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

namespace annotations
{
  struct symbol_table
    : annotation
  {
    typedef nodes::generic_node_ptr generic_node_ptr;

    void enter_scope (nodes::generic_node_ptr scope);
    void leave_scope ();

    bool insert (symbol_type type, std::string const &name, nodes::generic_node_ptr id);
    generic_node_ptr lookup (symbol_type type, std::string const &name);

    void clear () { assert (stack.empty ()); scopes.clear (); }
    void print () const;


    typedef std::map<std::string, generic_node_ptr> node_map;
    typedef std::vector<node_map> type_map;
    typedef std::map<generic_node_ptr, type_map> scope_map;

    std::vector<type_map *> stack;
    scope_map scopes;
  };
}
