#pragma once

#include <string>
#include <vector>
#include <utility>

struct graph
{
  typedef std::string				node_type;
  typedef std::pair<node_type, node_type>	edge_type;
  typedef std::vector<edge_type>		graph_type;

  void insert (std::string const &from, std::string const &to);
  std::vector<node_type> sorted () const;

  graph_type edges;
};
