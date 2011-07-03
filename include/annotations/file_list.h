#pragma once

#include "annotation.h"

#include <vector>

#include <boost/filesystem_fwd.hpp>

namespace annotations
{
  struct file_list
    : annotation
  {
    typedef std::vector<fs::path> file_vec;

    fs::path const &base;
    file_vec::const_iterator const begin;
    file_vec::const_iterator const end;

    file_list (fs::path const &base, file_vec::const_iterator it, file_vec::const_iterator et)
      : base (base)
      , begin (it)
      , end (et)
    {
    }
  };
}
