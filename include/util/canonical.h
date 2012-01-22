#pragma once

#include <algorithm>

#include "util/colours.h"

static char
underscore (char c)
{
  return isalnum (c) ? c : '_';
}

static std::string
canonical (std::string name, node_type current_symtype)
{
  transform (name.begin (), name.end (), name.begin (), underscore);
  if (current_symtype == n_program)
    return name;
  if (current_symtype == n_library)
    return "lib" + name + "_la";
  throw std::invalid_argument ("invalid state in target " + C::quoted (name));
}
