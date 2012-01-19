#include "fs/path.hpp"

#include <boost/filesystem/operations.hpp>

using fs::path;

#ifndef INLINE
#define INLINE inline
#endif

INLINE std::string const &fs::native (path const &p) { return p.data ().native (); }
INLINE char const *fs::c_str (path const &p) { return p.data ().c_str (); }

// decomposition
INLINE path fs::root_name (path const &p) { return fs::path (p.data ().root_name ()); }
INLINE path fs::root_directory (path const &p) { return fs::path (p.data ().root_directory ()); }
INLINE path fs::root_path (path const &p) { return fs::path (p.data ().root_path ()); }
INLINE path fs::relative_path (path const &p) { return fs::path (p.data ().relative_path ()); }
INLINE path fs::parent_path (path const &p) { return fs::path (p.data ().parent_path ()); }
INLINE path fs::filename (path const &p) { return fs::path (p.data ().filename ()); }
INLINE path fs::stem (path const &p) { return fs::path (p.data ().stem ()); }
INLINE path fs::extension (path const &p) { return fs::path (p.data ().extension ()); }

// query
INLINE bool fs::empty (path const &p) { return p.data ().empty (); }
INLINE bool fs::has_root_name (path const &p) { return p.data ().has_root_name (); }
INLINE bool fs::has_root_directory (path const &p) { return p.data ().has_root_directory (); }
INLINE bool fs::has_root_path (path const &p) { return p.data ().has_root_path (); }
INLINE bool fs::has_relative_path (path const &p) { return p.data ().has_relative_path (); }
INLINE bool fs::has_parent_path (path const &p) { return p.data ().has_parent_path (); }
INLINE bool fs::has_filename (path const &p) { return p.data ().has_filename (); }
INLINE bool fs::has_stem (path const &p) { return p.data ().has_stem (); }
INLINE bool fs::has_extension (path const &p) { return p.data ().has_extension (); }
INLINE bool fs::is_absolute (path const &p) { return p.data ().is_absolute (); }
INLINE bool fs::is_relative (path const &p) { return p.data ().is_relative (); }

INLINE path fs::operator / (path const &a, path const &b) { return fs::path (a.data () / b.data ()); }
INLINE path fs::operator / (path const &a, std::string const &b) { return fs::path (a.data () / b); }
INLINE path fs::operator / (std::string const &a, path const &b) { return fs::path (a / b.data ()); }

INLINE bool fs::operator == (path const &a, path const &b) { return a.data () == b.data (); }
INLINE bool fs::operator == (path const &a, std::string const &b) { return a.data () == b; }

INLINE bool fs::operator < (path const &a, path const &b) { return a.data () < b.data (); }

INLINE std::ostream &fs::operator << (std::ostream &os, path const &p) { return os << p.data (); }

INLINE bool fs::exists (path const &p) { return exists (p.data ()); }
INLINE bool fs::is_directory (path const &p) { return is_directory (p.data ()); }

INLINE size_t
fs::hash::operator () (fs::path const &path) const
{
#if STORE_PATH_HASH
  if (path.m.hash)
    return path.m.hash;
  return const_cast<size_t &> (path.m.hash) = std::tr1::hash<std::string> () (native (path));
#else
  return std::tr1::hash<std::string> () (native (path));
#endif
}

INLINE fs::path::m::m ()
#if STORE_PATH_HASH
  : hash (0)
#endif
{
}

INLINE fs::path::m::m (boost::filesystem::path data)
  : data (data)
#if STORE_PATH_HASH
  , hash (0)
#endif
{
}
