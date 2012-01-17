#include "node.h"

#include "foreach.h"
#include "object_pool.h"
#include "phases.h"

#include <cstdio>

#include <boost/filesystem/path.hpp>

static fs::path const generated_file = "<generated>";
location const location::generated = { &generated_file, 0, 0, 0, 0 };

namespace nodes
{
  void intrusive_ptr_release (node *n) { if (!--n->m.refcnt) delete n; }
  void intrusive_ptr_add_ref (node *n) { ++n->m.refcnt; }

  static object_pool<104> mempool;

  void *
  node::operator new (size_t bytes)
  {
    return ::operator new (bytes);
    return mempool.alloc (bytes);
  }

  void
  node::operator delete (void *ptr, size_t bytes)
  {
    return ::operator delete (ptr);
    return mempool.free (ptr, bytes);
  }


  std::vector<node *> node::nodes;

  node::node (location const &loc)
    : loc (loc)
    , m ()
  {
    m.index = nodes.size ();
    nodes.push_back (this);
  }

  node::~node ()
  {
    if (this == nodes.back ())
      nodes.pop_back ();
    else
      nodes[index ()] = 0;
  }

  void
  node::compress_hash ()
  {
    for (size_t i = 0; i < nodes.size (); i++)
      {
        if (!nodes[i])
          {
            while (!nodes.empty () && !nodes.back ())
              nodes.pop_back ();
            if (!nodes.empty ())
              {
                (nodes[i] = nodes.back ())->m.index = i;
                nodes.pop_back ();
              }
          }
      }

    assert (node_count () == hash_size ());
    assert (audit_hash ());
  }

  bool
  node::audit_hash ()
  {
    std::vector<node *>::const_reverse_iterator it = nodes.rbegin ();
    std::vector<node *>::const_reverse_iterator et = nodes.rend ();

    while (it != et)
      {
        if (*it && (*it)->index () != et - it - 1)
          return false;
        ++it;
      }
    return true;
  }

  static bool
  not_null (node *p)
  {
    return p != NULL;
  }

  size_t
  node::node_count ()
  {
    return count_if (nodes.begin (), nodes.end (), not_null);
  }

  size_t
  node::hash_size ()
  {
    return nodes.size ();
  }


  node_ptr
  node_list::clone_list (node_list const &orig, node_list_ptr clone)
  {
    assert (clone->list.empty ());

    clone->loc = orig.loc;
    clone->list.reserve (orig.size ());
    foreach (node_ptr const &n, orig.list)
      if (n)
        clone->add (n->clone ());
      else
        clone->add (0);

    return clone;
  }

  node_list *
  node_list::add (node_ptr n)
  {
    if (n)
      {
        assert (!n->parent ());
        n->m.parent = this;
        n->m.parent_index = list.size ();
      }
    list.push_back (n);
    return this;
  }

  node_list *
  node_list::set (size_t i, node_ptr n)
  {
    if (n)
      {
        if (n->parent ())
          n->parent ()->list.at (n->parent_index ()) = NULL;
        n->m.parent = this;
        n->m.parent_index = i;
      }
    list.at (i) = n;
    return this;
  }

  size_t
  node_list::size () const
  {
    return list.size ();
  }

  node_ptr const &node_list::operator [] (size_t index) const { return list.at (index); }

  node_list::node_list (location const &loc) : node (loc) { }

  node_list::~node_list ()
  {
    size_t i = 0;
    foreach (node_ptr const &n, list)
      {
        if (n)
          {
            assert (n->parent () == this);
            assert (n->parent_index () == i);
            n->m.parent = NULL;
          }
        ++i;
      }
  }

#include "node_cc.h"
}
