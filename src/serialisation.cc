#include "node.h"

#include "nodes.pb.h"
#include "util/foreach.h"

#include <fstream>

#include "fs/path.hpp"

#include <google/protobuf/text_format.h>

static struct s11n_files
{
  typedef std::map<std::string, fs::path const *> file_map;
  file_map files;

  fs::path const *add (std::string const &filename)
  {
    file_map::const_iterator found = files.find (filename);
    if (found == files.end ())
      found = files.insert (file_map::value_type (filename, new fs::path (filename))).first;
    return found->second;
  }

  static void destroy (std::pair<std::string, fs::path const *> const &pair)
  {
    delete pair.second;
  }

  ~s11n_files ()
  {
    google::protobuf::ShutdownProtobufLibrary ();
    for_each (files.begin (), files.end (), destroy);
  }
} files;

namespace nodes
{
  static void
  store_loc (Location &sloc, location const &loc)
  {
    sloc.set_file (native (*loc.file));
    sloc.set_first_line (loc.first_line);
    sloc.set_first_column (loc.first_column);
    sloc.set_last_line (loc.last_line);
    sloc.set_last_column (loc.last_column);
  }

  static void
  load_loc (location &loc, Location const &sloc)
  {
    loc.file = files.add (sloc.file ());
    loc.first_line = sloc.first_line ();
    loc.first_column = sloc.first_column ();
    loc.last_line = sloc.last_line ();
    loc.last_column = sloc.last_column ();
  }

  void
  node::store (std::ostream &os, node_ptr const &root, bool text)
  {
    compress_hash ();
    NodeList list;
    list.set_root (root->index () + 1);
    foreach (node *n, nodes)
      {
        Node &sn = *list.add_node ();
        sn.set_index (n->index () + 1);
        store_loc (*sn.mutable_loc (), n->loc);
        if (generic_node *p = n->is<generic_node> ())
          {
            GenericNode &list = *sn.mutable_node ();
            list.set_type (p->type);
            foreach (node_ptr const &c, p->list)
              if (c)
                list.add_child (c->index () + 1);
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

    if (!text)
      list.SerializeToOstream (&os);
    else
      {
        std::string str;
        google::protobuf::TextFormat::PrintToString (list, &str);
        os << str;
      }
  }

  node_ptr
  node::load (std::istream &is, bool text)
  {
    compress_hash ();
    assert (hash_size () == 0);
    NodeList list;

    if (!text)
      list.ParseFromIstream (&is);
    else
      {
        std::string str;
        is.seekg (0, std::ios::end);
        str.reserve (is.tellg ());
        is.seekg (0, std::ios::beg);
        str.assign (std::istreambuf_iterator<char> (is),
                    std::istreambuf_iterator<char> ());
        google::protobuf::TextFormat::ParseFromString (str, &list);
      }

    node_vec node_ptrs;
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
        assert (node_ptrs.size () == index);
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

    node_ptr const &root = node_ptrs[list.root () - 1];
#if 0
    foreach (node_ptr const &n, node_ptrs)
      if (&n != &root && n->refcnt == 1)
        {
          printf ("discarding node:\n---\n");
          phases::run ("xml", n);
          printf ("\n---\n");
        }
#endif

    assert (audit_hash ());
    return root;
  }
}
