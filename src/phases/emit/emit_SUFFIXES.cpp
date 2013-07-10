#include "phase.h"

#include "annotations/output_file.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/symbol_visitor.h"

#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

#include <boost/filesystem/path.hpp>

struct emit_SUFFIXES
  : symbol_visitor
{
  virtual void visit (t_document &n);

  virtual void visit (t_rule &n);


  typedef std::unordered_set<std::string> string_set;
  typedef std::unordered_map<std::string, string_set> suffix_map;

  output_file const &out;
  suffix_map suffix_rules;

  emit_SUFFIXES (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , out (annots.get ("output"))
  {
  }
};

static phase<emit_SUFFIXES> thisphase ("emit_SUFFIXES", "emit");

void
emit_SUFFIXES::visit (t_document &n)
{
  symbol_visitor::visit (n);

  fprintf (out.Makefile, "SUFFIXES =");
  foreach (suffix_map::const_reference rule, suffix_rules)
    fprintf (out.Makefile, " %s", rule.first.c_str ());
  fprintf (out.Makefile, "\n\n");

  foreach (suffix_map::const_reference rule, suffix_rules)
    foreach (std::string const &suffix, rule.second)
      fprintf (out.Makefile, "%s%s: ;\n", rule.first.c_str (), suffix.c_str ());

  fprintf (out.Makefile, "\n");
}


static std::string
suffix (std::string const &s)
{
  return fs::path (s).extension ().native ();
}

void
emit_SUFFIXES::visit (t_rule &n)
{
  std::string const &target = suffix (id (n.target ()->as<t_filenames> ()[0]->as<t_filename> ()[0]));
  // TODO: we don't know for sure which prereq was originally the pattern
  t_filenames &prereq = n.prereq ()->as<t_filenames> ();
  if (prereq.size () == 0)
    return;
  std::string const &source = suffix (id (prereq[0]->as<t_filename> ()[0]));

  if (!target.empty () && !source.empty ())
    suffix_rules[source].insert (target);
}
