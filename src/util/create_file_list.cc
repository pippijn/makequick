#include "util/create_file_list.h"

#include "foreach.h"
#include "ylcode.h"

#include <boost/filesystem/path.hpp>
#if 0
#include <boost/spirit/home/phoenix.hpp>
#endif

void
create_file_list (std::vector<fs::path> const &source_files,
                  nodes::node_vec &list,
                  location const &loc)
{
  using namespace nodes;
  using namespace tokens;
#if 0
  using namespace boost::phoenix;
  using namespace boost::phoenix::arg_names;
  transform (source_files.begin (),
             source_files.end (),
             back_inserter (list),
             new_<t_filename>
             ( loc,
               new_<token>
               ( loc,
                 TK_FILENAME,
                 bind (&fs::path::native, arg1))));
#endif
  foreach (fs::path const &p, source_files)
    list.push_back (new t_filename (loc, new token (loc, TK_FILENAME, p.native ())));
  //printf("%ld files => %ld nodes\n", source_files.size (), list.size ());
}
