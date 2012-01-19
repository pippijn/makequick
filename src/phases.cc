#include "phases.h"

#include "annotations/error_log.h"
#include "annotation_map.h"
#include "sighandler.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/timer.h"

#include <cstdio>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

typedef std::tr1::unordered_map<std::string, phases *> phase_map;

static phase_map map;

struct phases::pimpl
{
  std::tr1::unordered_set<std::string> dependencies;
};

void
phases::print_each (std::string const &name, phases *phase, node_ptr const &doc, annotation_map *annots)
{
  std::cout << "    " << name;
  if (!phase->autorun)
    std::cout << " (no autorun)";
  std::tr1::unordered_set<std::string> const &deps = phase->self->dependencies;
  if (!deps.empty ())
    std::cout << ':';
  foreach (std::string const &dep, deps)
    std::cout << ' ' << dep;
  std::cout << '\n';
}

void
phases::print ()
{
  for_each_phase (print_each);
}


phases::phases (std::string const &name, bool autorun)
  : autorun (autorun)
  , self (new pimpl)
{
  phases *&phase = map[name];
  if (phase)
    throw std::invalid_argument ("phase " + C::filename (name) + " already registered");
  phase = this;
}

phases::~phases ()
{
}

void
phases::run (std::string const &name, node_ptr const &doc, annotation_map &annots)
{
  phases *phase = map[name];
  if (!phase)
    throw std::invalid_argument ("phase " + C::filename (name) + " does not exist");

  printf ("%%%% phase \"%s\"\n", name.c_str ());
  phase->run1 (doc, annots);
}

void
phases::run (std::string const &name, node_ptr const &doc)
{
  annotation_map annots;
  run (name, doc, annots);
}

struct pair_equals
{
  pair_equals (std::string const &s)
    : s (s)
  {
  }

  bool operator () (std::pair<std::string, phases *> const &p)
  {
    return p.first == s;
  }

  std::string const &s;
};

void
phases::for_each_phase (phase_fn fn, node_ptr const &doc, annotation_map *annots)
{
  using namespace boost;

  typedef std::vector<std::pair<std::string, phases *> > phase_vec;
  phase_vec phases (map.size ());
  copy (map.begin (), map.end (),
        phases.begin ());

  typedef adjacency_list<vecS, vecS, directedS> Graph;
  Graph G (phases.size () + 1);
  for (phase_vec::const_reverse_iterator it = phases.rbegin (), et = phases.rend (); it != et; ++it)
    {
      add_edge (et - it, 0, G);
      foreach (std::string const &dep, it->second->self->dependencies)
        {
          if (!map[dep])
            throw std::invalid_argument ("phase " + C::filename (dep) + " does not exist");
          add_edge (et - it, et - find_if (phases.rbegin (), phases.rend (), pair_equals (dep)), G);
        }
    }

  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
  std::vector<Vertex> sorted;
  topological_sort (G, back_inserter (sorted));

  error_log const *errors = 0;
  if (annots)
    errors = &annots->get<error_log> ("errors");
  foreach (Vertex const &v, sorted)
    {
      if (should_terminate)
        return;
      if (v)
        {
          if (errors && errors->has_diagnostics ())
            return;
          std::string const &name = phases[v - 1].first;
          struct phases *phase    = phases[v - 1].second;
          fn (name, phase, doc, annots);

          // audit after each phase
          if (doc && annots)
            run ("audit", doc, *annots);
        }
    }
}

void
phases::run_each (std::string const &name, phases *phase, node_ptr const &doc, annotation_map *annots)
{
  assert (annots != NULL);
  if (phase->autorun)
    {
      printf ("%%%% phase \"%s\"\n", name.c_str ());
      phase->run1 (doc, *annots);
    }
}

void
phases::run (node_ptr const &doc, annotation_map &annots)
{
  for_each_phase (run_each, doc, &annots);
}

void
phases::add_dependency (char const *dependency)
{
  self->dependencies.insert (dependency);
}
