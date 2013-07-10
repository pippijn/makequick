#pragma once

#include <boost/filesystem.hpp>

#include <tr1/unordered_set>
#include <vector>

namespace fs = boost::filesystem;

typedef std::tr1::unordered_set<fs::path> file_set;
typedef std::vector<fs::path> file_vec;
