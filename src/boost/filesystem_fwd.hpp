#pragma once

#include <boost/filesystem.hpp>

#include <unordered_set>
#include <vector>

namespace fs = boost::filesystem;

typedef std::unordered_set<fs::path> file_set;
typedef std::vector<fs::path> file_vec;


template<typename T>
static inline size_t
hash (T const &v)
{
  return std::hash<T> () (v);
}


namespace std {

  template<>
  struct hash<fs::path>
  {
    size_t operator () (fs::path const &path) const
    {
      return ::hash (path.native ());
    }
  };

}
