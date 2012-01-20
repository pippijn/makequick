#pragma once

#include "annotation.h"

#include <string>
#include <map>

#include <boost/filesystem_fwd.hpp>

struct target_objects
  : annotation
{
  typedef std::map<std::string, file_vec> target_map;

  void print () const;

  target_map targets;
};
