#include "phase.h"

#include "annotations/output_file.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

struct emit_import
  : visitor
{
  emit_import (annotation_map &annots)
  {
    output_file const &out = annots.get ("output");
    if (exists (fs::path (out.base) / "Rules.am"))
      fprintf (out.Makefile, "include Rules.am\n");
  }
};

static phase<emit_import> thisphase ("emit_import", "emit");
