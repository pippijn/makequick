#pragma once

#include "annotation.h"

#include <string>
#include <map>
#include <vector>

#include "fs/fwd.hpp"

struct target_objects
  : annotation
{
  typedef std::map<std::string, std::vector<fs::path> > target_map;

  void print () const;

  target_map targets;
};
