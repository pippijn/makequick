#pragma once

#include "annotations/symbol_table.h"
#include "util/canonical.h"

#include <boost/filesystem/path.hpp>

static std::string
object_name (symbol_type type, std::string const &target_name, fs::path const &path)
{
  switch (type)
    {
    case T_LIBRARY:
      return canonical (target_name, n_library) + "-"
           + path.filename ().replace_extension (".lo").native ();
    case T_PROGRAM:
      return canonical (target_name, n_program) + "-"
           + path.filename ().replace_extension (".o").native ();
    default:
      throw std::runtime_error ("invalid sources container");
    }
}
