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
    node (location const &loc);
    virtual ~node ();

    void *operator new (size_t bytes);
    void operator delete (void *ptr, size_t bytes);

    location loc;

    template<typename T> T &as () { return dynamic_cast<T &> (*this); }
    template<typename T> T *is () { return dynamic_cast<T *> ( this); }

    static void store (std::ostream &os, node_ptr const &root, bool text = false);
    static node_ptr load (std::istream &is, bool text = false);

    int refcnt;
    int index;

    static std::vector<node *> nodes;

    static void compress_hash ();
    static bool audit_hash ();
    static size_t node_count ();
    static size_t hash_size ();
  };

  struct node_list
    : node
  {
    node_list *add (node_ptr n) { list.push_back (n); return this; }
    size_t size () const { return list.size (); }
    node_ptr       &operator [] (size_t index)       { return list.at (index); }
    node_ptr const &operator [] (size_t index) const { return list.at (index); }

    node_list (location const &loc) : node (loc) { }

    node_vec list;
  };


  struct generic_node
    : node_list
  {
    generic_node (int type, location const &loc) : node_list (loc), type (type) { }
    generic_node (int type, location const &loc, node_ptr n1) : node_list (loc), type (type) {
      add (n1);
    }
    generic_node (int type, location const &loc, node_ptr n1, node_ptr n2) : node_list (loc), type (type) {
      add (n1);
      add (n2);
    }
    generic_node (int type, location const &loc, node_ptr n1, node_ptr n2, node_ptr n3) : node_list (loc), type (type) {
      add (n1);
      add (n2);
      add (n3);
    }
    generic_node (int type, location const &loc, node_ptr n1, node_ptr n2, node_ptr n3, node_ptr n4) : node_list (loc), type (type) {
      add (n1);
      add (n2);
      add (n3);
      add (n4);
    }
    generic_node (int type, location const &loc, node_ptr n1, node_ptr n2, node_ptr n3, node_ptr n4, node_ptr n5) : node_list (loc), type (type) {
      add (n1);
      add (n2);
      add (n3);
      add (n4);
      add (n5);
    }

    int type;
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

namespace tokens
{
  using nodes::node;
  using nodes::visitor;

  struct token
    : node
  {
    virtual void accept (visitor &v) { v.visit (*this); }
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
