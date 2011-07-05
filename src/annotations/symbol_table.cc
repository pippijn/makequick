#include "annotations/symbol_table.h"

#include "phases.h"
#include "node.h"
#include "node_type.h"

using annotations::symbol_table;
using nodes::generic_node_ptr;

void
symbol_table::enter_scope (generic_node_ptr scope)
{
#if 0
  puts ("enter_scope");
#endif
  stack.push_back (&scopes[scope]);
}

void
symbol_table::leave_scope ()
{
#if 0
  puts ("leave_scope");
#endif
  stack.pop_back ();
}

static symbol_table::node_map &
get_scope (symbol_table::type_map &map, symbol_type type)
{
  if (map.size () <= type)
    map.resize (type + 1);
  return map[type];
}

bool
symbol_table::insert (symbol_type type, std::string const &name, generic_node_ptr id)
{
  if (stack.empty ())
    return false;

#if 0
  printf ("insert (%s)\n", name.c_str ());
#endif

  node_map &scope = get_scope (*stack.back (), type);

  if (scope[name])
    return false;
  scope[name] = id;
  return true;
}

generic_node_ptr
symbol_table::lookup (symbol_type type, std::string const &name)
{
  generic_node_ptr resolved;
  size_t scope = stack.size ();
#if 0
  printf ("lookup: %d scopes for %s\n", scope, name.c_str ());
#endif
  while (!resolved && scope)
    resolved = get_scope (*stack[--scope], type)[name];
  return resolved;
}

static char const *
symbol_type_name (int type)
{
  switch (type)
    {
    case T_VARIABLE: return "variable";
    }

  throw std::runtime_error ("invalid symbol type");
}

void
symbol_table::print () const
{
  using nodes::node_type_name;

  puts ("symbol table:");
  BOOST_FOREACH (scope_map::const_reference scope, scopes)
    {
      printf ("scope for %s[%d]: ", node_type_name[scope.first->type], scope.first->index);
      phases::run ("name", scope.first);
      BOOST_FOREACH (node_map const &type, scope.second)
        if (!type.empty ())
          {
            printf ("  type %s\n", symbol_type_name (scope.second.size () - (&scope.second.back () - &type) - 1));
            BOOST_FOREACH (node_map::const_reference sym, type)
              {
                printf ("    sym %s (%s[%d])\n", sym.first.c_str (), node_type_name[sym.second->type], sym.second->index);
              }
          }
    }
}