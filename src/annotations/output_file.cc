#include "annotations/output_file.h"

#include <cassert>

using annotations::output_file;

output_file::output_file (char const *mname, char const *cname)
  : Makefile (fopen (mname, "w"))
  , configure (fopen (cname, "w"))
{
  assert (Makefile);
  assert (configure);
}

output_file::~output_file ()
{
  fclose (configure);
  fclose (Makefile);
}
