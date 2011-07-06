#pragma once

#include "node_ptr.h"

#include <boost/filesystem_fwd.hpp>

node_ptr parse_files (std::vector<fs::path> const &files, int init = 0, bool alternative = false);
node_ptr parse_string (std::string const &s, int init = 0, bool alternative = false);
