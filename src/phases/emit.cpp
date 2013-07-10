#include "config.h"
#include "phase.h"

#include "annotations/output_file.h"
#include "util/foreach.h"

struct emit
  : visitor
{
  annotation_map &annots;
  output_file const &out;

  void visit (t_document &n);

  emit (annotation_map &annots)
    : annots (annots)
    , out (annots.get ("output"))
  {
  }

#if 0
  ~emit ()
  {
    if (!std::uncaught_exception ())
      throw 0;
  }
#endif
};

static phase<emit> thisphase ("emit", "merge_blocks");


void
emit::visit (t_document &n)
{
  FILE *prelude = fopen (PKGDATADIR "/prelude.am", "r");
  if (prelude == NULL)
    throw 0;

  while (!feof (prelude))
    {
      char buf[64];
      fwrite (buf, 1, fread (buf, 1, sizeof buf, prelude), out.Makefile);
    }

  fclose (prelude);

  fprintf (out.Makefile, "\n");

  phases::run ("emit_m4", &n, annots);
  phases::run ("emit_dirs", &n, annots);
  phases::run ("emit_test", &n, annots);
  phases::run ("emit_SUFFIXES", &n, annots);
  phases::run ("emit_headers", &n, annots);
  phases::run ("emit_built_sources", &n, annots);
  phases::run ("emit_targets", &n, annots);
  phases::run ("emit_link", &n, annots);
  phases::run ("emit_flags", &n, annots);
  phases::run ("emit_SOURCES", &n, annots);
  phases::run ("emit_rules", &n, annots);

  phases::run ("emit_import", &n, annots);
}
