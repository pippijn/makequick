#include "util/colours.h"

std::string
C::filename (fs::path const &fn)
{
  return filename (native (fn));
}
