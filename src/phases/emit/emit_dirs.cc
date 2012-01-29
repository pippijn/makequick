#include "phase.h"

#include "annotations/output_file.h"
#include "util/extract_string.h"

struct emit_dirs
  : visitor
{
  virtual void visit (t_dir &n);

  output_file const &out;

  emit_dirs (annotation_map &annots)
    : out (annots.get ("output"))
  {
  }
};

static phase<emit_dirs> thisphase ("emit_dirs", "emit");


void
emit_dirs::visit (t_dir &n)
{
  fprintf (out.Makefile, "%sdir = %s\n",
           id (n.name ()).c_str (),
           extract_string (*n.dir ()).c_str ());
}
