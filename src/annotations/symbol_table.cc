#include "annotations/symbol_table.h"

#include "foreach.h"
#include "phases.h"
#include "node.h"

#include <cstdio>
#include <stdexcept>

using nodes::generic_node_ptr;

symbol_table::symbol_table ()
{
  stack.push_back (&scopes[0]);
}

void
symbol_table::enter_scope (generic_node_ptr const &scope)
{
#if 0
  puts ("enter_scope");
#endif
  // cannot re-enter the global scope
  assert (scope);
  stack.push_back (&scopes[scope]);
  current_scope = scope;
}

generic_node_ptr
symbol_table::leave_scope ()
{
  // needs at least the global scope
  assert (stack.size () > 1);
#if 0
  puts ("leave_scope");
#endif
  stack.pop_back ();
  generic_node_ptr scope = current_scope;
  current_scope = 0;
  return scope;
}

symbol_table::node_map &
symbol_table::get_scope (symbol_table::type_map &map, symbol_type type)
{
  if (map.size () <= type)
    map.resize (type + 1);
  return map[type];
}

bool
symbol_table::insert (node_map &scope, std::string const &name, generic_node_ptr const &id)
{
  assert (id);

  if (scope[name])
    return false;
  scope[name] = id;
  return true;
}

bool
symbol_table::insert (symbol_type type, std::string const &name, generic_node_ptr const &id)
{
  if (stack.empty ())
    return false;

#if 0
  printf ("insert (%s)\n", name.c_str ());
#endif

  return insert (get_scope (*stack.back (), type), name, id);
}

bool
symbol_table::insert_global (symbol_type type, std::string const &name, generic_node_ptr const &id)
{
  return insert (get_scope (*stack[0], type), name, id);
}

generic_node_ptr
symbol_table::lookup (symbol_type type, std::string const &name) const
{
  assert (!stack.empty ());

  size_t scope = stack.size ();
#if 0
  printf ("lookup: %d scopes for %s\n", scope, name.c_str ());
#endif

  do
    {
      node_map const &map = get_scope (*stack[--scope], type);
      node_map::const_iterator found = map.find (name);
      if (found != map.end ())
        return found->second;
    }
  while (scope);

  return 0;
}

static char const *
symbol_type_name (size_t type)
{
  switch (type)
    {
    case T_VARIABLE: return "variable";
    case T_PROGRAM : return "program";
    case T_LIBRARY : return "library";
    case T_TEMPLATE: return "template";
    }

  throw std::runtime_error ("invalid symbol type");
}

using nodes::node_type_name;

void
symbol_table::print (symbol_table::node_map const &type, char const *type_name)
{
  if (!type.empty ())
    {
      printf ("    type %s\n", type_name);
      foreach (symbol_table::node_map::value_type const &sym, type)
        {
          printf ("      sym %s (%s[%d])\n", sym.first.c_str (), node_type_name[sym.second->type], sym.second->index);
        }
    }
}

static char const *
node_type_name_opt (generic_node_ptr const &p)
{
  return nodes::node_type_name[p->type];
}

void
symbol_table::print (symbol_table::scope_map::value_type const &scope)
{
  if (scope.first)
    printf ("  scope for %s[%d]\n", node_type_name[scope.first->type], scope.first->index);
  else
    printf ("  global scope\n");
  foreach (symbol_table::node_map const &type, scope.second)
    print (type, symbol_type_name (scope.second.size () - (&scope.second.back () - &type) - 1));
}

void
symbol_table::print () const
{
  puts ("symbol table:");
  foreach (scope_map::value_type const &scope, scopes)
    print (scope);
}

void
symbol_table::print_stack () const
{
  int indent = 0;
  foreach (symbol_table::type_map const *s, stack)
    {
      foreach (scope_map::value_type const &scope, scopes)
        if (&scope.second == s)
          printf ("%*s%s\n", indent * 2, "", node_type_name_opt (scope.first));
      indent++;
    }
}
