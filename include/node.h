#pragma once

#include <string>
#if NODE_USE_LIST
#include <list>
#else
#include <vector>
#endif

#include <boost/filesystem.hpp>
#include <boost/intrusive_ptr.hpp>

#include "visitor.h"

namespace fs = boost::filesystem;

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
    node (location const &loc) : loc (loc), refcnt (0) { }
    virtual ~node () { }

    friend void intrusive_ptr_release (node *n);
    friend void intrusive_ptr_add_ref (node *n);

    void *operator new (size_t bytes);
    void operator delete (void *ptr, size_t bytes);

    location loc;

    template<typename T>
    T &as () { return dynamic_cast<T &> (*this); }

  private:
    int refcnt;
  };

  struct node_list
    : node
  {
    void add (node_ptr n) { list.push_back (n); }
    size_t size () const { return list.size (); }
    node_ptr &operator [] (size_t index) { return list.at (index); }

    node_list (location const &loc) : node (loc) { }

#if NODE_USE_LIST
    std::list<node_ptr> list;
#else
    std::vector<node_ptr> list;
#endif
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

using nodes::node;
using nodes::node_ptr;
