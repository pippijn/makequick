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
  fprintf (out.Makefile, "AUTOMAKE_OPTIONS	= -Wall -Wno-override\n");

  // pkg-config output
  fprintf (out.Makefile, "pkgconfigdir		= $(libdir)/pkgconfig\n");
  fprintf (out.Makefile, "pkgconfig_DATA	= # empty\n");
  fprintf (out.Makefile, "\n");

  // Allow for += later
  fprintf (out.Makefile, "bin_PROGRAMS		= # empty\n");
  fprintf (out.Makefile, "bin_SCRIPTS		= # empty\n");
  fprintf (out.Makefile, "check_LTLIBRARIES	= # empty\n");
  fprintf (out.Makefile, "check_PROGRAMS	= # empty\n");
  fprintf (out.Makefile, "data_DATA		= # empty\n");
  fprintf (out.Makefile, "include_HEADERS	= # empty\n");
  fprintf (out.Makefile, "lib_LTLIBRARIES	= # empty\n");
  fprintf (out.Makefile, "man_MANS		= # empty\n");
  fprintf (out.Makefile, "noinst_DATA		= # empty\n");
  fprintf (out.Makefile, "noinst_HEADERS	= # empty\n");
  fprintf (out.Makefile, "noinst_LTLIBRARIES	= # empty\n");
  fprintf (out.Makefile, "noinst_PROGRAMS	= # empty\n");
  fprintf (out.Makefile, "pkgdata_DATA		= # empty\n");
  fprintf (out.Makefile, "pkglibexec_PROGRAMS	= # empty\n");
  fprintf (out.Makefile, "pkglib_LTLIBRARIES	= # empty\n");
  fprintf (out.Makefile, "\n");

  fprintf (out.Makefile, "TESTS			= # empty\n");
  fprintf (out.Makefile, "XFAIL_TESTS		= # empty\n");
  fprintf (out.Makefile, "\n");

  phases::run ("emit_test", &n, annots);
  phases::run ("emit_SUFFIXES", &n, annots);
  phases::run ("emit_built_sources", &n, annots);
  phases::run ("emit_targets", &n, annots);
  phases::run ("emit_link", &n, annots);
  phases::run ("emit_flags", &n, annots);
  phases::run ("emit_SOURCES", &n, annots);
  phases::run ("emit_rules", &n, annots);

  // Standard includes come last, so targets have the option to override them
  fprintf (out.Makefile, "AM_CPPFLAGS		+= -I$(top_srcdir) -I$(top_srcdir)/include\n");
  fprintf (out.Makefile, "AM_CPPFLAGS		+= -I$(top_builddir) -I$(top_builddir)/include\n");
}
