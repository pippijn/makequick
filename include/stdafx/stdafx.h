#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/regex.hpp>
#include <boost/spirit/home/phoenix.hpp>

namespace fs = boost::filesystem;
#define foreach BOOST_FOREACH
