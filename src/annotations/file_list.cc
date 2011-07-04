#include "annotations/file_list.h"

using annotations::file_list;

void
file_list::print () const
{
  for (vector::const_iterator it = begin; it != end; ++it)
    std::cout << '{' << base.native () << "/}" << rel (*it).native () << '\n';
}

fs::path
file_list::rel (fs::path const &file) const
{
  return file.native ().substr (base.native ().length () + 1);
}

file_list::file_list (fs::path const &base, iterator it, iterator et)
  : base (base)
  , begin (it)
  , end (et)
{
  for (vector::const_iterator it = begin; it != end; ++it)
    if (it->native ().substr (0, base.native ().length ()) != base.native ())
      throw std::runtime_error ("found filename outside source root: " + it->native ());
}
