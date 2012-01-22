#include "util/graph.h"
#include "util/foreach.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

#include <cstdio>
#include <tr1/unordered_map>

void
graph::insert (std::string const &from, std::string const &to)
{
  edges.push_back (edge_type (from, to));
}


std::vector<graph::node_type>
graph::sorted () const
{
  using namespace boost;

  std::vector<node_type> nodes;
  foreach (edge_type const &edge, edges)
    {
      nodes.push_back (edge.first);
      nodes.push_back (edge.second);
    }
  sort (nodes.begin (), nodes.end ());
  nodes.erase (unique (nodes.begin (), nodes.end ()), nodes.end ());

  typedef adjacency_list<vecS, vecS, directedS> Graph;
  Graph G (edges.size ());

  foreach (edge_type const &edge, edges)
    {
      size_t from = find (nodes.begin (), nodes.end (), edge.first ) - nodes.begin ();
      size_t to   = find (nodes.begin (), nodes.end (), edge.second) - nodes.begin ();
#if 0
      printf ("%ld[%s] -> %ld[%s]\n",
              from, edge.first.c_str (),
              to, edge.second.c_str ());
#endif
      assert (nodes[from] == edge.first);
      assert (nodes[to] == edge.second);
      add_edge (from, to, G);
    }

  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
  std::vector<Vertex> sorted;
  sorted.reserve (nodes.size ());
  topological_sort (G, back_inserter (sorted));
#if 0
  printf ("sorted size: %ld, input set size: %ld\n", sorted.size (), nodes.size ());
  assert (sorted.size () == nodes.size ());
#endif

  std::vector<node_type> result;
  foreach (Vertex const &v, sorted)
    {
      if (v < nodes.size ())
        {
#if 0
          printf ("%ld[%s]\n", v, nodes[v].c_str ());
#endif
          result.push_back (nodes[v]);
        }
    }

  foreach (node_type const &node, nodes)
    assert (find (result.begin (), result.end (), node) != result.end ());
  foreach (node_type const &node, result)
    assert (find (nodes.begin (), nodes.end (), node) != nodes.end ());

  return result;
}
