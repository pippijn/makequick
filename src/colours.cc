#include "util/colours.h"

#include <boost/filesystem/path.hpp>

std::string
C::filename (fs::path const &fn)
{
  return filename (fn.native ());
}
