#pragma once

#include "annotation.h"
#include "node_ptr.h"

#include <string>
#include <vector>

#include <boost/filesystem_fwd.hpp>

namespace annotations
{
  struct rule_info
    : annotation
  {
    struct rule
    {
      std::string target;
      std::vector<std::string> prereq;
      std::string stem;
      node_ptr code;
    };

    rule const *find (std::string const &target) const;

    std::vector<fs::path> files;
    std::vector<rule> rules;
  };
}
