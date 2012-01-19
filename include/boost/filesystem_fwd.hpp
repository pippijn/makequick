#pragma once

#include <tr1/unordered_set>

namespace boost
{
  namespace filesystem3
  {
    struct path;
  }
}

namespace fs = boost::filesystem3;

typedef std::tr1::unordered_set<fs::path> file_set;
