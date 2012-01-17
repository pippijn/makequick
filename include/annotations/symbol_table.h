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
  typedef nodes::generic_node_ptr node_ptr;

  symbol_table ();

  void enter_scope (node_ptr const &scope);
  void leave_scope ();

  bool insert (symbol_type type, std::string const &name, node_ptr const &sym);
  bool insert_global (symbol_type type, std::string const &name, node_ptr const &sym);
  node_ptr lookup (symbol_type type, std::string const &name) const;
  bool remove (symbol_type type, std::string const &name, node_ptr const &sym);

  void clear () { assert (stack.empty ()); scopes.clear (); }
  void print () const;
  void print_stack () const;

private:
  typedef std::map<std::string, node_ptr> node_map;
  typedef std::vector<node_map> type_map;
  typedef std::map<node_ptr, type_map> scope_map;

  typedef scope_map::const_iterator scope_iterator;
  typedef type_map::const_iterator type_iterator;
  typedef node_map::const_iterator node_iterator;

  std::vector<type_map *> stack;
  scope_map scopes;

  static symbol_table::node_map &get_scope (symbol_table::type_map &map, symbol_type type);
  static bool insert (node_map &scope, std::string const &name, node_ptr const &sym);

  static void print (symbol_table::node_map const &type, char const *type_name);
  static void print (symbol_table::scope_map::value_type const &scope);

  symbol_table (symbol_table const &other);

public:
  class iterator
  {
    scope_iterator scope_it;
    type_iterator type_it;
    node_iterator node_it;

    scope_iterator const scope_et;

  public:
    struct value_type
    {
      node_ptr const &scope;
      symbol_type const type;
      std::string const &name;
      node_ptr const &sym;

      value_type (node_ptr const &scope,
                  symbol_type const type,
                  std::string const &name,
                  node_ptr const &sym)
        : scope (scope)
        , type (type)
        , name (name)
        , sym (sym)
      {
      }

      friend std::ostream &operator << (std::ostream &os, value_type const &val);

      value_type *operator -> () { return this; }
    };

    iterator (scope_iterator scope_it, scope_iterator scope_et, type_iterator type_it, node_iterator node_it)
      : scope_it (scope_it)
      , type_it (type_it)
      , node_it (node_it)
      , scope_et (scope_et)
    {
    }

    value_type operator *  () const
    {
      return value_type (scope_it->first,
                         symbol_type (type_it - scope_it->second.begin ()),
                         node_it->first,
                         node_it->second);
    }

    value_type operator -> () const { return operator * (); }

    iterator &operator ++ ();
    iterator &operator -- ();

    bool operator == (iterator const &rhs) const;
    bool operator != (iterator const &rhs) const { return !(*this == rhs); }
  };

  iterator begin () const;
  iterator end () const;

  struct filtering_iterator_base
  {
    typedef iterator::value_type value_type;

    filtering_iterator_base (symbol_type filter, iterator begin, iterator it, iterator end)
      : filter (filter)
      , begin (begin)
      , it (it)
      , end (end)
    {
    }

    value_type      operator *  () const { return *it; }
    iterator const &operator -> () const { return  it; }

    filtering_iterator_base &operator ++ ()
    {
      while (++it != end && it->type != filter)
        ;
      return *this;
    }

    filtering_iterator_base &operator -- ()
    {
      while (--it != begin && it->type != filter)
        ;
      return *this;
    }

    bool operator == (filtering_iterator_base const &rhs) const { return it == rhs.it; }
    bool operator != (filtering_iterator_base const &rhs) const { return it != rhs.it; }

  private:
    symbol_type const filter;
    iterator const begin;
    iterator it;
    iterator const end;
  };

  template<symbol_type Filter>
  struct filtering_iterator
    : filtering_iterator_base
  {
    typedef filtering_iterator_base base;

    filtering_iterator (iterator begin, iterator it, iterator end)
      : filtering_iterator_base (Filter, begin, it, end)
    {
    }

    filtering_iterator &operator ++ () { base::operator ++ (); return *this; }
    filtering_iterator &operator -- () { base::operator -- (); return *this; }
  };

  template<symbol_type Filter>
  filtering_iterator<Filter> begin () const
  {
    iterator it = begin (), et = end ();
    find_filtered_iterators (it, et, Filter);
    return filtering_iterator<Filter> (it, it, et);
  }

  template<symbol_type Filter>
  filtering_iterator<Filter> end () const
  {
    iterator it = begin (), et = end ();
    find_filtered_iterators (it, et, Filter);
    return filtering_iterator<Filter> (it, et, et);
  }

private:
  static void find_valid_begin (scope_iterator &scope_it,
                                scope_iterator const scope_et,
                                type_iterator &type_it,
                                node_iterator &node_it);
  static void find_valid_end (scope_iterator &scope_it,
                              scope_iterator const scope_et,
                              type_iterator &type_it,
                              node_iterator &node_it);

  static void find_filtered_iterators (iterator &it, iterator &et, symbol_type filter);
};
