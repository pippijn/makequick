#pragma once

#include <string>

#include "fs/fwd.hpp"

namespace C
{
  static inline std::string
  red (std::string const &fn)
  {
    return "\e[1;31m" + fn + "\e[0m";
  }

  static inline std::string
  yellow (std::string const &fn)
  {
    return "\e[0;33m" + fn + "\e[0m";
  }

  static inline std::string
  YELLOW (std::string const &fn)
  {
    return "\e[1;33m" + fn + "\e[0m";
  }

  static inline std::string
  blue (std::string const &fn)
  {
    return "\e[0;36m" + fn + "\e[0m";
  }


  static inline std::string
  quoted (std::string const &fn)
  {
    return yellow ("`" + fn + "'");
  }

  static inline std::string
  filename (std::string const &fn)
  {
    return quoted (fn);
  }

  std::string filename (fs::path const &fn);
}
