#pragma once

#include <string>

#include <boost/filesystem_fwd.hpp>

namespace C
{
  static inline std::string
  filename (std::string const &fn)
  {
    return "\e[0;33m`" + fn + "'\e[0m";
  }

  std::string filename (fs::path const &fn);

  static inline std::string
  red (std::string const &fn)
  {
    return "\e[1;31m" + fn + "\e[0m";
  }

  static inline std::string
  yellow (std::string const &fn)
  {
    return "\e[1;33m" + fn + "\e[0m";
  }
}
