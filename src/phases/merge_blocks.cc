#include "phase.h"

#include "foreach.h"

#include <map>

struct if_compare
{
  bool operator () (t_if_ptr const &a, t_if_ptr const &b) const
  {
    if (!a && !b) return false;
    if (!a) return true;
    if (!b) return false;
    return a->cond ()->as<token> ().string
         < b->cond ()->as<token> ().string
         ;
  }
};

template<typename NodeT, node_ptr const &(NodeT::*items) () const, typename BodyT>
struct merger
{
  typedef typename boost::intrusive_ptr<NodeT> node_t_ptr;
  typedef typename boost::intrusive_ptr<BodyT> body_t_ptr;

  void visit (NodeT &n)
  {
    t_if_ptr cond = n.cond () ? &n.cond ()->as<t_if> () : 0;
    if (node_t_ptr block = blocks[cond])
      {
        BodyT &body = ((*block).*items) ()->as<BodyT> ();
        foreach (node_ptr const &p, (n.*items) ()->as<BodyT> ().list)
          body.add (p); // moves body to other block
        n.unlink ();
      }
    else
      {
        blocks[cond] = &n;
      }
  }

  void clear ()
  {
    blocks.clear ();
  }

  std::map<t_if_ptr, node_t_ptr, if_compare> blocks;
};


struct merge_blocks
  : visitor
{
  void visit (t_target_definition &n);
  void visit (t_link &n);
  void visit (t_extra_dist &n);

  merge_blocks (annotation_map &annots)
  {
  }

  merger<t_link, &t_link::items, t_link_body> link_merger;
  merger<t_extra_dist, &t_extra_dist::sources, t_sources_members> extra_dist_merger;
};

static phase<merge_blocks> thisphase ("merge_blocks", "inheritance");


void
merge_blocks::visit (t_target_definition &n)
{
  link_merger.clear ();
  visitor::visit (n);
}

void
merge_blocks::visit (t_link &n)
{
  link_merger.visit (n);
}

void
merge_blocks::visit (t_extra_dist &n)
{
  // extra_dist is not reset in visit(target_definition), as it's global
  extra_dist_merger.visit (n);
}
