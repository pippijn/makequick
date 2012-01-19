#include "node.h"

#include "object_pool.h"
#include "phases.h"
#include "util/foreach.h"
#include "util/unlink.h"

#include <cstdio>

#include <boost/filesystem/path.hpp>

static fs::path const generated_file = "<generated>";
location const location::generated = { &generated_file, 0, 0, 0, 0 };
location const location::invalid = { 0, 0, 0, 0, 0 };

bool
operator == (location const &a, location const &b)
{
  return a.file == b.file
      && a.first_line == b.first_line
      && a.first_column == b.first_column
      && a.last_line == b.last_line
      && a.last_column == b.last_column;
}

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
    assert (loc != location::invalid);
    m.index = nodes.size ();
    m.parent_index = -1;
    nodes.push_back (this);
  }

  node::~node ()
  {
    if (this == nodes.back ())
      nodes.pop_back ();
    else
      nodes[index ()] = 0;
  }


  node_ptr const &
  node::prev_sibling () const
  {
    assert (parent ());
    return (*parent ())[parent_index () - 1];
  }

  node_ptr const &
  node::next_sibling () const
  {
    assert (parent ());
    return (*parent ())[parent_index () + 1];
  }


  bool
  node::has_prev_sibling () const
  {
    return parent () && parent_index () > 0;
  }

  bool
  node::has_next_sibling () const
  {
    return parent () && parent_index () < parent ()->size () - 1;
  }


  void
  node::unlink ()
  {
    // remove self from parent
    if (parent ())
      {
        assert (parent_index () != -1);
        parent ()->list.at (parent_index ()) = NULL;
        m.parent = NULL;
        m.parent_index = -1;
      }
    // invariant: no parent => parent_index == -1
    assert (parent_index () == -1);
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

  static void
  set (node_ptr const &n, node_list &list, size_t i)
  {
    // set value in list
    if (i == list.size ())
      list.list.push_back (n);
    else
      list.list.at (i) = n;
  }

  static void
  move (node_ptr const &n, node_list &list, size_t i)
  {
    if (!n)
      return set (0, list, i);

    // move away from original parent
    n->unlink ();
    // set list as new parent
    n->m.parent = &list;
    n->m.parent_index = i;

    set (n, list, i);
  }

  node_list *
  node_list::add (node_ptr n)
  {
    move (n, *this, size ());
    return this;
  }

  node_list *
  node_list::set (size_t i, node_ptr n)
  {
    assert (i < size ());
    assert (n->parent () || n->parent_index () == -1);
    move (n, *this, i);
    return this;
  }

  void
  node_list::replace (size_t index, node_vec::const_iterator from, node_vec::const_iterator to)
  {
    size_t additional_size = distance (from, to);
    size_t old_size = list.size ();
    size_t new_size = old_size - 1 + additional_size;
    assert (new_size >= old_size);
#if 0

    printf ("replacing [%ld] in #%ld with %ld nodes\n", index, this->index (), additional_size);
    for (size_t i = 0; i < additional_size; i++)
      printf ("[%ld] = #%ld\n", i, from[i]->index ());

    // make space for new elements
    list.resize (new_size);

    // move all existing nodes past `index' to the end
    for (size_t old_index = old_size - 1; old_index > index; old_index--)
      {
        size_t new_index = old_index - 1 + additional_size;
        // don't need to move
        if (new_index == old_index)
          continue;

        assert (list.size () > new_index);
        assert (!list[new_index]);

        // move node ahead
        std::swap (list[old_index], list[new_index]);

        // update parent_index
        if (list[new_index])
          list[new_index]->m.parent_index = new_index;
      }

    copy (from, to, list.begin () + index);
    for (size_t i = index; i < index + additional_size; i++)
      {
        list.at (i)->m.parent = this;
        list.at (i)->m.parent_index = i;
      }
#endif
#if 0
    for (size_t i = 0; i < additional_size; i++)
      {
        assert (from != to);
        node_ptr n = *from;
        if (n)
          {
            assert (n->parent () || n->parent_index () == -1);
            n->unlink ();
            assert (!n->parent () && n->parent_index () == -1);
          }
        set (index + i, n);
        ++from;
      }
#endif

    // naive, inefficient but working way:
    node_vec const &body = unlink_all (list);

    std::vector<node_ptr>::const_iterator it = body.begin ();
    std::vector<node_ptr>::const_iterator et = body.end ();

    for (size_t i = 0; i < index; i++)
      add (*it++);

    while (from != to)
      add (*from++);
    
    while (++it != et)
      add (*it);

    assert (size () == new_size);
    assert (audit_list ());
  }

  bool
  node_list::audit_list () const
  {
    for (size_t i = 0; i < size (); i++)
      if (list[i])
        {
          assert (list[i]->parent_index () == i);
          if (list[i]->parent_index () != i)
            return false;
        }
    return true;
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
