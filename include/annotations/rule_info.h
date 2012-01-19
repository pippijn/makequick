#pragma once

#include "annotation.h"
#include "node_ptr.h"

#include <string>
#include <vector>

#include "fs/fwd.hpp"

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

  file_set files;
  std::vector<rule> rules;
};
