#pragma once

#include "node_ptr.h"

#include <boost/filesystem_fwd.hpp>

struct file_list;
struct error_log;

node_ptr parse_files (file_list const &files, error_log &errors, int init = 0, bool alternative = false);
node_ptr parse_string (std::string const &s, error_log &errors, int init = 0, bool alternative = false);
