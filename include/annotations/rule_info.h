#pragma once

#include "annotation.h"
#include "node_ptr.h"

#include <string>
#include <vector>

#include <boost/filesystem_fwd.hpp>
#include <boost/filesystem/path.hpp>

struct rule_info
  : annotation
{
  struct rule
  {
    std::string target;
    std::vector<fs::path> prereq;
    std::string stem;
    node_ptr code;

    void print () const;

    rule (std::string const &target, std::vector<fs::path> const &prereq, std::string const &stem, node_ptr code)
      : target (target)
      , prereq (prereq)
      , stem (stem)
      , code (code)
    {
    }
  };

  rule const *find (std::string const &target) const;

  file_set files;
  std::vector<rule> rules;
};
