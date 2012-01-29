#include "phase.h"

#include "annotations/output_file.h"
#include "annotations/symbol_table.h"
#include "util/colours.h"
#include "util/foreach.h"
#include "util/graph.h"
#include "util/grep.h"
#include "util/symbol_visitor.h"
#include "util/plus_writer.h"

#include <stdexcept>

struct emit_targets
  : symbol_visitor
  , plus_writer
{
  virtual void visit (t_document &n);
  virtual void visit (t_destination &n);
  virtual void visit (t_target_definition &n);

  struct target
  {
    std::string name;
    std::string cond;

    target ()
    {
    }

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

  void print_targets (FILE *out, emit_targets::target_map const &targets,
                      char const *kind, std::string makeise (std::string const &s));

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

void
emit_targets::print_targets (FILE *out, emit_targets::target_map const &targets,
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

          plus (out, pair.first, kind);
          fprintf (out, "%s\n",
                   makeise (it->name).c_str ());

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

  // topologically sort libraries
  foreach (emit_targets::target_map::value_type &pair, libraries)
    if (pair.second.size () > 1)
      {
        graph G;
        std::map<std::string, target> targets;
        foreach (target const &lib, pair.second)
          {
            targets[lib.name] = lib;
            t_target_definition &target = symtab.lookup<t_target_definition> (T_LIBRARY, lib.name);

            bool has_dependencies = false;
            foreach (t_link &links, grep<t_link> (target.body ()))
              foreach (token &link, grep<token> (links.items ()))
                if (link.tok == TK_INT_LIB)
                  {
                    G.insert (lib.name, link.string);
                    has_dependencies = true;
                  }
            // if it has no dependencies, it won't appear in the graph otherwise
            if (!has_dependencies)
              G.insert (lib.name, std::string ());
          }
        std::vector<std::string> const &sorted = G.sorted ();
        foreach (target const &lib, pair.second)
          if (find (sorted.begin (), sorted.end (), lib.name) == sorted.end ())
            throw std::runtime_error ("sorted list did not contain `" + lib.name + "'");
        pair.second.clear ();
        foreach (std::string const &target, sorted)
          if (!target.empty ())
            pair.second.push_back (targets[target]);
      }

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
