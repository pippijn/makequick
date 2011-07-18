#pragma once

#include "node_ptr.h"

#include <boost/filesystem_fwd.hpp>

namespace annotations
{
  struct error_log;
}

node_ptr parse_files (std::vector<fs::path> const &files, annotations::error_log &errors, int init = 0, bool alternative = false);
node_ptr parse_string (std::string const &s, annotations::error_log &errors, int init = 0, bool alternative = false);
