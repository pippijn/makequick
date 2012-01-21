#include "annotations/output_file.h"

#include <cassert>
#include <cstring>

static char const sig[] = "#$MQ$ Generated - do not modify";

static FILE *
open_if_mine (char const *name)
{
  FILE *fh = fopen (name, "r");
  if (fh)
    {
      char check[sizeof sig] = { 0 };
      fread (check, 1, sizeof check - 1, fh);
      fclose (fh);
      if (strcmp (sig, check))
        return NULL;
    }
  fh = fopen (name, "w");
  if (!fh)
    return NULL;
  fprintf (fh, "%s\n", sig);
  fflush (fh);
  return fh;
}

output_file::output_file (char const *mname, char const *cname)
  : Makefile (open_if_mine (mname))
  , configure (open_if_mine (cname))
{
  assert (Makefile);
  assert (configure);
}

output_file::~output_file ()
{
  fclose (configure);
  fclose (Makefile);
}
