#include "annotations/symbol_table.h"

#include "foreach.h"
#include "phases.h"
#include "node.h"

#include <cstdio>
#include <stdexcept>

symbol_table::symbol_table ()
{
  // the table needs at least one level for begin() and end() to work
  type_map &scope = scopes[0];
  scope.push_back (node_map ());
  stack.push_back (&scope);
}

void
symbol_table::enter_scope (node_ptr const &scope)
{
  // cannot re-enter the global scope
  assert (scope);
#if 0
  puts ("enter_scope");
#endif
  stack.push_back (&scopes[scope]);
}

void
symbol_table::leave_scope ()
{
  // needs at least the global scope
  assert (stack.size () > 1);
#if 0
  puts ("leave_scope");
#endif
  stack.pop_back ();
}

symbol_table::node_map &
symbol_table::get_scope (symbol_table::type_map &map, symbol_type type)
{
  if (map.size () <= type)
    map.resize (type + 1);
  return map[type];
}

bool
symbol_table::insert (node_map &scope, std::string const &name, node_ptr const &sym)
{
  assert (sym);

  if (scope[name])
    return false;
  scope[name] = sym;
  return true;
}

bool
symbol_table::insert (symbol_type type, std::string const &name, node_ptr const &sym)
{
  if (stack.empty ())
    return false;

#if 0
  printf ("insert (%s)\n", name.c_str ());
#endif

  return insert (get_scope (*stack.back (), type), name, sym);
}

bool
symbol_table::insert_global (symbol_type type, std::string const &name, node_ptr const &sym)
{
  return insert (get_scope (*stack[0], type), name, sym);
}

symbol_table::node_ptr
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

bool
symbol_table::remove (symbol_type type, std::string const &name, node_ptr const &sym)
{
  assert (!stack.empty ());

  size_t scope = stack.size ();
#if 0
  printf ("lookup: %d scopes for %s\n", scope, name.c_str ());
#endif

  do
    {
      node_map &map = get_scope (*stack[--scope], type);
      node_map::iterator found = map.find (name);
      if (found != map.end ())
        {
          assert (found->second == sym);
          map.erase (found);
          return true;
        }
    }
  while (scope);

  return false;
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
          printf ("      sym %s (%s[%d])\n", sym.first.c_str (), node_type_name[sym.second->type], sym.second->index ());
        }
    }
}

static char const *
node_type_name_opt (symbol_table::node_ptr const &p)
{
  return nodes::node_type_name[p->type];
}

void
symbol_table::print (symbol_table::scope_map::value_type const &scope)
{
  if (scope.first)
    printf ("  scope for %s[%d]\n", node_type_name[scope.first->type], scope.first->index ());
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

std::ostream &
operator << (std::ostream &os, symbol_table::iterator::value_type const &val)
{
  os << "{ scope: ";
  if (val.scope)
    os << node_type_name[val.scope->type] << '[' << val.scope->index () << ']';
  else
    os << "<global>";
  os << ", type: \"" << symbol_type_name (val.type) << '"'
     << ", name: \"" << val.name << '"'
     << ", sym: " << node_type_name[val.sym->type] << '[' << val.sym->index () << ']'
     << " }";

  return os;
}

symbol_table::iterator &
symbol_table::iterator::operator ++ ()
{
  assert (scope_it != scope_et);

  ++node_it;
  if (node_it == type_it->end ())
    {
      ++type_it;
      if (type_it == scope_it->second.end ())
        {
          ++scope_it;
          if (scope_it == scope_et)
            return *this;
          type_it = scope_it->second.begin ();
        }
      node_it = type_it->begin ();
    }

  return *this;
}

symbol_table::iterator &
symbol_table::iterator::operator -- ()
{
  if (scope_it == scope_et)
    --scope_it;
  if (type_it == scope_it->second.end ())
    --type_it;

  if (node_it == type_it->begin ())
    {
      if (type_it == scope_it->second.begin ())
        {
          --scope_it;
          type_it = scope_it->second.end ();
        }
      --type_it;
      node_it = type_it->end ();
    }
  --node_it;

  return *this;
}

bool
symbol_table::iterator::operator == (iterator const &rhs) const
{
  assert (scope_et == rhs.scope_et);
  return scope_it == rhs.scope_it
      &&  type_it == rhs. type_it
      &&  node_it == rhs. node_it;
}


void
symbol_table::find_valid_begin (scope_iterator &scope_it,
                                scope_iterator const scope_et,
                                type_iterator &type_it,
                                node_iterator &node_it)
{
  while (scope_it != scope_et)
    {
      type_it = scope_it->second.begin ();
      while (type_it != scope_it->second.end ())
        {
          node_it = type_it->begin ();
          if (node_it != type_it->end ())
            return;
          ++type_it;
        }
      ++scope_it;
    }
}

symbol_table::iterator
symbol_table::begin () const
{
  scope_iterator scope_it = scopes.begin ();
  scope_iterator scope_et = scopes.end ();
  type_iterator type_it;
  node_iterator node_it;

  find_valid_begin (scope_it, scope_et, type_it, node_it);

  return iterator (scope_it, scope_et, type_it, node_it);
}


void
symbol_table::find_valid_end (scope_iterator &scope_it,
                              scope_iterator const scope_et,
                              type_iterator &type_it,
                              node_iterator &node_it)
{
  scope_iterator const begin = scope_it;
  scope_it = scope_et;

  assert (scope_it != begin);
    {
      scope_iterator last = scope_it;
      --last;
      type_it = last->second.end ();
      assert (type_it != last->second.begin ());
        {
          type_iterator last = type_it;
          --last;
          node_it = last->end ();
        }
    }
}

symbol_table::iterator
symbol_table::end () const
{
  scope_iterator scope_it = scopes.begin ();
  scope_iterator scope_et = scopes.end ();
  type_iterator type_it;
  node_iterator node_it;

  find_valid_end (scope_it, scope_et, type_it, node_it);

  return iterator (scope_it, scope_et, type_it, node_it);
}


void
symbol_table::find_filtered_iterators (iterator &it, iterator &et, symbol_type filter)
{
  iterator const end = et;

  while (it != et && it->type != filter)
    ++it;
  while (et != it && (--et)->type != filter)
    ;
  if (et != end)
    ++et;
}
