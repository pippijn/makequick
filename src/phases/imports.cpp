#include "config.h"
#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/file_list.h"
#include "parseutil.h"

#include <boost/filesystem/operations.hpp>

struct imports
  : visitor
{
  void visit (t_extern &n);

  std::tr1::unordered_set<std::string> imported;

  bool seen (std::string const &import)
  {
    if (imported.find (import) != imported.end ())
      return true;
    imported.insert (import);
    return false;
  }

  imports (annotation_map &annots)
  {
  }
};

static phase<imports> thisphase ("imports");


static fs::path
unquote (std::string const &quoted)
{
  assert (!quoted.empty ());
  assert (*quoted. begin () == '"');
  assert (*quoted.rbegin () == '"');

  return quoted.substr (1, quoted.length () - 2);
}

void
imports::visit (t_extern &n)
{
  fs::path const base   = RULESDIR;
  fs::path const import = unquote (id (n.import ())).replace_extension (".mq");

  if (seen (import.native ()))
    return;

  if (!exists (base / import))
    throw import.c_str ();

  file_vec singleton;
  singleton.push_back (import);

  file_list list (base, singleton.begin (), singleton.end ());
  error_log errors;
  if (node_ptr result = parse_files (list, errors))
    {
      n.parent ()->replace (n,
                            result->as<t_document> ()
                            .decls ()->as<t_toplevel_declarations> ()
                            .list);
    }
}
