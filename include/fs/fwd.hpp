#pragma once

#include <string>
#include <tr1/unordered_set>

namespace boost
{
  namespace filesystem3
  {
    struct path;
  }
}

namespace fs
{
  using namespace boost::filesystem3;
  struct path;

  std::string const &native (path const &p);
  char const *c_str (path const &p);

  // decomposition
  path root_name (path const &p);
  path root_directory (path const &p);
  path root_path (path const &p);
  path relative_path (path const &p);
  path parent_path (path const &p);
  path filename (path const &p);
  path stem (path const &p);
  path extension (path const &p);

  // query
  bool empty (path const &p);
  bool has_root_name (path const &p);
  bool has_root_directory (path const &p);
  bool has_root_path (path const &p);
  bool has_relative_path (path const &p);
  bool has_parent_path (path const &p);
  bool has_filename (path const &p);
  bool has_stem (path const &p);
  bool has_extension (path const &p);
  bool is_absolute (path const &p);
  bool is_relative (path const &p);

  path operator / (path const &a, path const &b);
  path operator / (path const &a, std::string const &b);
  path operator / (std::string const &a, path const &b);

  bool operator == (path const &a, path const &b);
  bool operator == (path const &a, std::string const &b);

  bool operator < (path const &a, path const &b);

  std::ostream &operator << (std::ostream &os, path const &p);

  // operations
  bool exists (path const &p);
  bool is_directory (path const &p);

  struct hash
  {
    size_t operator () (path const &p) const;
  };
}

typedef std::tr1::unordered_set<fs::path, fs::hash> file_set;
