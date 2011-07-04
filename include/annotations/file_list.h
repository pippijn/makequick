#pragma once

#include "annotation.h"

#include <vector>

#include <boost/filesystem_fwd.hpp>

namespace annotations
{
  struct file_list
    : annotation
  {
    typedef std::vector<fs::path> vector;
    typedef vector::const_iterator iterator;

    fs::path const &base;
    iterator const begin;
    iterator const end;

    void print () const;
    fs::path rel (fs::path const &file) const;

    file_list (fs::path const &base, iterator it, iterator et);
  };
}
