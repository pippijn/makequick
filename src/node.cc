#include "node.h"

#include "foreach.h"
#include "nodes.pb.h"
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


  static std::vector<node *> nodes;

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

  static void
  store_loc (Location &sloc, location const &loc)
  {
    sloc.set_file (loc.file->native ());
    sloc.set_first_line (loc.first_line);
    sloc.set_first_column (loc.first_column);
    sloc.set_last_line (loc.last_line);
    sloc.set_last_column (loc.last_column);
  }

  static void
  load_loc (location &loc, Location const &sloc)
  {
    //loc.file = &sloc.file ();
    loc.first_line = sloc.first_line ();
    loc.first_column = sloc.first_column ();
    loc.last_line = sloc.last_line ();
    loc.last_column = sloc.last_column ();
  }

  void
  node::store (std::ostream &os, node_ptr const &root)
  {
    compress_hash ();
    NodeList list;
    list.set_root (root->index + 1);
    foreach (node *n, nodes)
      {
        Node &sn = *list.add_node ();
        sn.set_index (n->index + 1);
        store_loc (*sn.mutable_loc (), n->loc);
        if (generic_node *p = n->is<generic_node> ())
          {
            GenericNode &list = *sn.mutable_node ();
            list.set_type (p->type);
            foreach (node_ptr const &c, p->list)
              if (c)
                list.add_child (c->index + 1);
              else
                list.add_child (0);
          }
        else
          {
            tokens::token &t = n->as<tokens::token> ();
            Token &token = *sn.mutable_token ();
            token.set_tok (t.tok);
            token.set_str (t.string);
          }
      }
    list.SerializeToOstream (&os);
  }

  node_ptr
  node::load (std::istream &is)
  {
    compress_hash ();
    assert (hash_size () == 0);
    NodeList list;
    list.ParseFromIstream (&is);

    std::vector<node_ptr> node_ptrs;
    foreach (Node const &sn, list.node ())
      {
        int const index = sn.index ();
        node *n;
        if (sn.has_node ())
          {
            GenericNode const &list = sn.node ();
            node_list *p = make_node (node_type (list.type ()));
            n = p;
          }
        else
          {
            Token const &token = sn.token ();
            tokens::token *t = new tokens::token ();
            t->tok = token.tok ();
            t->mutable_string = token.str ();
            n = t;
          }
        load_loc (n->loc, sn.loc ());
        assert (nodes.size () == index);
        node_ptrs.push_back (n);
      }

    // fix up references
    foreach (Node const &sn, list.node ())
      {
        if (sn.has_node ())
          {
            generic_node &n = nodes[sn.index () - 1]->as<generic_node> ();
            GenericNode const &list = sn.node ();
            foreach (int c, list.child ())
              n.add (c ? nodes[c - 1] : 0);
          }
      }

    assert (audit_hash ());
    return nodes[list.root () - 1];
  }
}
