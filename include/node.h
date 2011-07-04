#pragma once

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

    template<typename T> T   &as () { return dynamic_cast<T &> (*this); }
    template<typename T> bool is () { return dynamic_cast<T *> ( this); }

    int refcnt;
    int index;

    static void compress_hash ();
    static bool audit_hash ();
    static size_t node_count ();
  };

  struct node_list
    : node
  {
    node_list *add (node_ptr n) { list.push_back (n); return this; }
    size_t size () const { return list.size (); }
    node_ptr &operator [] (size_t index) { return list.at (index); }

    node_list (location const &loc) : node (loc) { }

    node_vec list;
  };


  struct generic_node
    : node_list
  {
    virtual void accept (visitor &v) { v.visit (*this); }

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

    int const type;
  };
}

namespace tokens
{
  using nodes::node;
  using nodes::visitor;

  struct token
    : node
  {
    virtual void accept (visitor &v) { v.visit (*this); }
    token (location const &loc, int tok, char const *text, int leng)
      : node (loc)
      , tok (tok)
      , string (text, leng)
    {
    }
    token (location const &loc, int tok, std::string const &string)
      : node (loc)
      , tok (tok)
      , string (string)
    {
    }

    int const tok;
    std::string const string;
  };
}
