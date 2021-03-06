#include "phase.h"

#include "util/ancestor.h"
#include "util/foreach.h"

#include <map>

struct if_compare
{
  bool operator () (t_if_ptr const &a, t_if_ptr const &b) const
  {
    if (!a && !b) return false;
    if (!a) return true;
    if (!b) return false;
    return id (a->cond ())
         < id (b->cond ())
         ;
  }
};

template<typename T>
static t_if_ptr
get_cond (T &n)
{
  return n.cond () ? &n.cond ()->as<t_if> () : NULL;
}

template<>
t_if_ptr
get_cond<t_built_sources> (t_built_sources &n)
{
  return NULL;
}

template<typename NodeT, node_ptr const &(NodeT::*items) () const, typename BodyT>
struct merger
{
  typedef typename boost::intrusive_ptr<NodeT> node_t_ptr;
  typedef typename boost::intrusive_ptr<BodyT> body_t_ptr;

  void visit (NodeT &n)
  {
    t_if_ptr cond = get_cond (n);
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
  void visit (t_sources &n);
  void visit (t_tool_flags &n);

  void visit (t_extra_dist &n);
  void visit (t_built_sources &n);
  void visit (t_test &n);

  merge_blocks (annotation_map &annots)
  {
  }

  merger<t_link, &t_link::items, t_link_body> link_merger;
  merger<t_sources, &t_sources::sources, t_sources_members> sources_merger;
  std::map<std::string, merger<t_tool_flags, &t_tool_flags::flags, t_flag> > tool_flags_mergers;

  merger<t_extra_dist, &t_extra_dist::sources, t_sources_members> extra_dist_merger;
  merger<t_built_sources, &t_built_sources::sources, t_sources_members> built_sources_merger;
  std::map<std::string, merger<t_test, &t_test::sources, t_sources_members> > test_mergers;
};

static phase<merge_blocks> thisphase ("merge_blocks", "expand_vars");


void
merge_blocks::visit (t_target_definition &n)
{
  // extra_dist and built_sources are not reset, as they are global
  link_merger.clear ();
  sources_merger.clear ();
  tool_flags_mergers.clear ();
  test_mergers.clear ();
  visitor::visit (n);
}


void
merge_blocks::visit (t_link &n)
{
  link_merger.visit (n);
}

void
merge_blocks::visit (t_sources &n)
{
  sources_merger.visit (n);
}

void
merge_blocks::visit (t_tool_flags &n)
{
  // TODO: move this elsewhere
  if (!n.flags ())
    {
      n.unlink ();
      return;
    }
  assert (n.flags ());
  if (ancestor<t_target_definition> (n))
    tool_flags_mergers[id (n.keyword ())].visit (n);
}


void
merge_blocks::visit (t_extra_dist &n)
{
  extra_dist_merger.visit (n);
}

void
merge_blocks::visit (t_built_sources &n)
{
  built_sources_merger.visit (n);
}

static std::string
id_opt (node_ptr const &p)
{
  return p ? id (p) : std::string ();
}

void
merge_blocks::visit (t_test &n)
{
  test_mergers[id_opt (n.type ())].visit (n);
}
