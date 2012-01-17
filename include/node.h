#pragma once

#include "node_type.h"
#include "visitor.h"

#include <string>

#include <boost/filesystem_fwd.hpp>

struct location
{
  fs::path const *file;
  int first_line;
  int first_column;
  int last_line;
  int last_column;

  static location const generated;
};

namespace nodes
{
  struct node
  {
    virtual void accept (visitor &v) = 0;
    virtual node_ptr clone () const = 0;
    node (location const &loc);
    virtual ~node ();

    void *operator new (size_t bytes);
    void operator delete (void *ptr, size_t bytes);

    location loc;

    template<typename T> T &as () { return dynamic_cast<T &> (*this); }
    template<typename T> T *is () { return dynamic_cast<T *> ( this); }

    static void store (std::ostream &os, node_ptr const &root, bool text = false);
    static node_ptr load (std::istream &is, bool text = false);

    struct m
    {
      int refcnt;
      int index;
      int parent_index;
      node_list *parent;
    } m;

    int refcnt () const { return m.refcnt; }
    int index () const { return m.index; }
    int parent_index () const { return m.parent_index; }
    node_list *parent () const { return m.parent; }

    static std::vector<node *> nodes;

    static void compress_hash ();
    static bool audit_hash ();
    static size_t node_count ();
    static size_t hash_size ();
  };

  struct node_list
    : node
  {
    static node_ptr clone_list (node_list const &orig, node_list_ptr clone);

    node_list *add (node_ptr n);
    node_list *set (size_t i, node_ptr n);
    size_t size () const;
    node_ptr const &operator [] (size_t index) const;

    node_list (location const &loc);
    ~node_list ();

    node_vec list;
  };


  struct generic_node
    : node_list
  {
    generic_node (int type, location const &loc) : node_list (loc), type (type) { }

    int const type;
  };

  template<typename Derived>
  struct generic_node_t
    : generic_node
  {
    generic_node_t (int type, location const &loc) : generic_node (type, loc) { }
    generic_node_t (int type, location const &loc, node_ptr n1) : generic_node (type, loc) {
      add (n1);
    }
    generic_node_t (int type, location const &loc, node_ptr n1, node_ptr n2) : generic_node (type, loc) {
      add (n1);
      add (n2);
    }
    generic_node_t (int type, location const &loc, node_ptr n1, node_ptr n2, node_ptr n3) : generic_node (type, loc) {
      add (n1);
      add (n2);
      add (n3);
    }
    generic_node_t (int type, location const &loc, node_ptr n1, node_ptr n2, node_ptr n3, node_ptr n4) : generic_node (type, loc) {
      add (n1);
      add (n2);
      add (n3);
      add (n4);
    }
    generic_node_t (int type, location const &loc, node_ptr n1, node_ptr n2, node_ptr n3, node_ptr n4, node_ptr n5) : generic_node (type, loc) {
      add (n1);
      add (n2);
      add (n3);
      add (n4);
      add (n5);
    }

    virtual node_ptr clone () const
    {
      return clone_list (*this, new Derived);
    }
  };


  template<node_type Type>
  node_list *make_node (location const &loc);

  template<node_type Type>
  node_list *make_node (location const &loc, node_ptr const &v0);

  template<node_type Type>
  node_list *make_node (location const &loc, node_ptr const &v0, node_ptr const &v1);

  template<node_type Type>
  node_list *make_node (location const &loc, node_ptr const &v0, node_ptr const &v1, node_ptr const &v2);

  template<node_type Type>
  node_list *make_node (location const &loc, node_ptr const &v0, node_ptr const &v1, node_ptr const &v2, node_ptr const &v3);

  template<node_type Type>
  node_list *make_node (location const &loc, node_ptr const &v0, node_ptr const &v1, node_ptr const &v2, node_ptr const &v3, node_ptr const &v4);

#include "node_t.h"

  node_list *make_node (node_type type);

  template<typename T>
  boost::intrusive_ptr<T>
  as (node_ptr const &n)
  {
    return &n->as<T> ();
  }
}

static inline bool
operator < (nodes::generic_node_ptr const &a, nodes::generic_node_ptr const &b)
{
  if (!a)
    return true;
  if (!b)
    return false;
  return a->index () < b->index ();
}


namespace tokens
{
  using nodes::node;
  using nodes::visitor;

  struct token
    : node
  {
    virtual void accept (visitor &v) { v.visit (*this); }
    virtual node_ptr clone () const { return new token (loc, tok, string); }
    token ()
      : node (location::generated)
      , string (mutable_string)
    {
    }
    token (location const &loc, int tok, char const *text, int leng)
      : node (loc)
      , tok (tok)
      , mutable_string (text, leng)
      , string (mutable_string)
    {
    }
    token (location const &loc, int tok, std::string const &string)
      : node (loc)
      , tok (tok)
      , mutable_string (string)
      , string (mutable_string)
    {
    }

    int tok;
    std::string mutable_string;
    std::string const &string;
  };
}
