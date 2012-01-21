#include "phase.h"

#include "annotations/output_file.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/symbol_visitor.h"

#include <stdexcept>

struct emit_targets
  : symbol_visitor
{
  virtual void visit (t_document &n);
  virtual void visit (t_destination &n);
  virtual void visit (t_target_definition &n);

  struct target
  {
    std::string name;
    std::string cond;

    target (std::string const &name, std::string const &cond)
      : name (name)
      , cond (cond)
    {
    }
  };

  typedef std::map<std::string, std::vector<target> > target_map;
  target_map programs;
  target_map libraries;
  output_file const &out;
  std::string cond;
  bool in_target;

  emit_targets (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , out (annots.get ("output"))
    , in_target (false)
  {
  }
};

static phase<emit_targets> thisphase ("emit_targets", "emit");


static void
tabbed (FILE *out, std::string const &s)
{
  fprintf (out, "\t\\\n\t%s", s.c_str ());
}

static void
print_targets (FILE *out, emit_targets::target_map const &targets,
               char const *kind, std::string makeise (std::string const &s))
{
  foreach (emit_targets::target_map::const_reference pair, targets)
    {
      std::vector<emit_targets::target>::const_iterator it = pair.second.begin ();
      std::vector<emit_targets::target>::const_iterator et = pair.second.end ();

      std::string cond;
      while (it != et)
        {
          if (cond != it->cond)
            {
              if (!cond.empty ())
                fprintf (out, "endif\n");
              if (!it->cond.empty ())
                fprintf (out, "if %s\n", it->cond.c_str ());
            }

          fprintf (out, "%s_%s += %s\n",
                   pair.first.c_str (), kind, makeise (it->name).c_str ());

          cond = it->cond;
          ++it;
        }
      if (!cond.empty ())
        fprintf (out, "endif\n");

      fprintf (out, "\n");
    }
}

static std::string program_makeise (std::string const &s) { return s; }

static std::string library_makeise (std::string const &s) { return "lib" + s + ".la"; }

void
emit_targets::visit (t_document &n)
{
  symbol_visitor::visit (n);
  print_targets (out.Makefile, programs, "PROGRAMS", program_makeise);
  fprintf (out.Makefile, "\n");
  print_targets (out.Makefile, libraries, "LTLIBRARIES", library_makeise);
  fprintf (out.Makefile, "\n");
}

void
emit_targets::visit (t_destination &n)
{
  if (!in_target)
    return;

  t_target_definition &TARGET = symtab.lookup<t_target_definition> (T_PROGRAM, T_LIBRARY, "TARGET");
  std::string const &name = id (TARGET.name ());
  std::string const &dir  = id (n.dir ());

  if (current_symtype == n_program)
    programs[dir].push_back (target (name, cond));
  else if (current_symtype == n_library)
    libraries[dir].push_back (target (name, cond));
}

void
emit_targets::visit (t_target_definition &n)
{
  in_target = true;
  cond = n.cond () ? id (n.cond ()->as<t_if> ().cond ()) : "";
  symbol_visitor::visit (n);
  cond.clear ();
  in_target = false;
}
