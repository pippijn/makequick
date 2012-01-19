#pragma once

#include "annotation.h"

#include <vector>

#include "fs/fwd.hpp"

struct file_list
  : annotation
{
  typedef std::vector<fs::path> vector;
  typedef vector::const_iterator iterator;

  fs::path const &base;
  iterator const begin;
  iterator const end;

  void print () const;
  fs::path absolute (fs::path const &file) const;

  file_list (fs::path const &base, iterator it, iterator et);
};
