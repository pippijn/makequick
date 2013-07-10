#include "config.h"
#include "phase.h"

#include "annotations/output_file.h"

#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

struct emit_m4
  : visitor
{
  static void link (fs::path const &m4, fs::path const &file)
  {
    fs::path const &target = m4 / file.filename ();
    printf ("%%%% linking %s\n", target.c_str ());
    remove (target);
    create_symlink (file, target);
  }

  emit_m4 (annotation_map &annots)
  {
    fs::path const &m4 = fs::path (annots.get<output_file> ("output").base) / "m4";
    create_directory (m4);
    std::for_each (fs::directory_iterator (M4DIR),
                   fs::directory_iterator (),
                   bind (link, boost::ref (m4), _1));
  }
};

static phase<emit_m4> thisphase ("emit_m4", "emit");
