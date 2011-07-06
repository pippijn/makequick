#pragma once

#include "node_ptr.h"

#include <vector>

#include <boost/filesystem_fwd.hpp>

void create_file_list (std::vector<fs::path> const &source_files,
                       std::vector<node_ptr> &list,
                       location const &loc);
