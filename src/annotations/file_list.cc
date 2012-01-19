#include "annotations/file_list.h"

#include "fs/path.hpp"

void
file_list::print () const
{
  for (vector::const_iterator it = begin; it != end; ++it)
    std::cout << '{' << native (base) << "/}" << native (*it) << '\n';
}

fs::path
file_list::absolute (fs::path const &file) const
{
  return base / file;
}

file_list::file_list (fs::path const &base, iterator it, iterator et)
  : base (base)
  , begin (it)
  , end (et)
{
}
