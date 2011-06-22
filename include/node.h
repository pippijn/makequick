#pragma once

#include <string>
#include <vector>

#include <boost/intrusive_ptr.hpp>

#include "visitor.h"

struct YYLTYPE
{
  std::string const *file;
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1

namespace nodes
{
  struct node
  {
    virtual void accept (visitor &v) = 0;
    node () : refcnt (0) { }
    virtual ~node () { }

    friend void intrusive_ptr_release (node *n);
    friend void intrusive_ptr_add_ref (node *n);

    void *operator new (size_t bytes);
    void operator delete (void *ptr, size_t bytes);

  private:
    int refcnt;
  };

  typedef boost::intrusive_ptr<node> node_ptr;

  struct node_list
    : node
  {
    void add (node_ptr n) { list.push_back (n); }

    std::vector<node_ptr> list;
  };

  struct token
    : node
  {
    virtual void accept (visitor &v) { v.visit (*this); }
    token (int tok, char const *text, int leng)
      : tok (tok)
      , string (text, leng)
    {
    }
    token (int tok, std::string const &string)
      : tok (tok)
      , string (string)
    {
    }

    int const tok;
    std::string const string;
  };


  struct generic_node
    : node_list
  {
    virtual void accept (visitor &v) { v.visit (*this); }

    generic_node (char const *name) : name (name) { }
    generic_node (char const *name, node_ptr n1) : name (name) {
      add (n1);
    }
    generic_node (char const *name, node_ptr n1, node_ptr n2) : name (name) {
      add (n1);
      add (n2);
    }
    generic_node (char const *name, node_ptr n1, node_ptr n2, node_ptr n3) : name (name) {
      add (n1);
      add (n2);
      add (n3);
    }

    char const *name;
  };
}

using nodes::node;
using nodes::node_ptr;
