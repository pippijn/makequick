#include "node.h"

#include "object_pool.h"

#include <cstdio>

#include <boost/filesystem/path.hpp>

static fs::path const generated_file = "<generated>";
location const location::generated = { &generated_file, 0, 0, 0, 0 };

namespace nodes
{
  void intrusive_ptr_release (node *n) { if (!--n->refcnt) delete n; }
  void intrusive_ptr_add_ref (node *n) { ++n->refcnt; }

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
    , refcnt (0)
  {
    index = nodes.size ();
    nodes.push_back (this);
  }

  node::~node ()
  {
    if (this == nodes.back ())
      nodes.pop_back ();
    else
      nodes[index] = 0;
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
                (nodes[i] = nodes.back ())->index = i;
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
        if (*it && (*it)->index != et - it - 1)
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
}
