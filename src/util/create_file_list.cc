#include "util/create_file_list.h"

#include "node_type.h"
#include "ylcode.h"

void
create_file_list (std::vector<fs::path> const &source_files,
                  std::vector<node_ptr> &list,
                  location const &loc)
{
  using namespace nodes;
  using namespace tokens;
  using namespace boost::phoenix;
  using namespace boost::phoenix::arg_names;
  std::transform (source_files.begin (),
                  source_files.end (),
                  std::back_inserter (list),
                  new_<generic_node>
                  ( n_filename,
                    loc,
                    new_<token>
                    ( loc,
                      TK_FILENAME,
                      bind (&fs::path::native, arg1))));
}

