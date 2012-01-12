#include "phase.h"

#include "annotations/output_file.h"
#include "colours.h"
#include "foreach.h"
#include "util/symbol_visitor.h"

#include <stdexcept>

struct emit_targets
  : symbol_visitor
{
  virtual void visit (t_document &n);
  virtual void visit (t_if &n);
  virtual void visit (t_destination &n);

  virtual void visit (token &n);

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
  annotations::output_file const &out;
  std::string cond;

  emit_targets (annotation_map &annots)
    : symbol_visitor (annots.get<symbol_table> ("symtab"))
    , out (annots.get ("output"))
  {
  }
};

static phase<emit_targets> thisphase ("emit_targets", noauto);


static void
tabbed (FILE *out, std::string const &s)
{
  fprintf (out, "\t\\\n\t%s", s.c_str ());
}

void
emit_targets::visit (t_document &n)
{
  visitor::visit (n);
  foreach (target_map::const_reference pair, programs)
    {
      std::vector<target>::const_iterator it = pair.second.begin ();
      std::vector<target>::const_iterator et = pair.second.end ();

next:
      fprintf (out.Makefile, "%s_PROGRAMS +=", pair.first.c_str ());
      while (it != et)
        {
          if (!it->cond.empty ())
            {
              fprintf (out.Makefile, "\nif %s\n%s_PROGRAMS += %s\nendif",
                       it->cond.c_str (),
                       pair.first.c_str (),
                       it->name.c_str ());
              if (++it != et)
                {
                  fprintf (out.Makefile, "\n");
                  goto next;
                }
            }
          else
            {
              tabbed (out.Makefile, it->name);
              ++it;
            }
        }
      fprintf (out.Makefile, "\n");
    }
  fprintf (out.Makefile, "\n");
  foreach (target_map::const_reference pair, libraries)
    {
      fprintf (out.Makefile, "%s_LTLIBRARIES +=", pair.first.c_str ());
      foreach (target const &lib, pair.second)
        tabbed (out.Makefile, "lib" + lib.name + ".la");
      fprintf (out.Makefile, "\n");
    }
  fprintf (out.Makefile, "\n");
}

void
emit_targets::visit (t_if &n)
{
  cond = n.cond () ? n.cond ()->as<token> ().string : "";
}

void
emit_targets::visit (t_destination &n)
{
  generic_node_ptr TARGET = symtab.lookup (T_VARIABLE, "TARGET");
  assert (TARGET);
  generic_node &name_node = TARGET->as<generic_node> ();
  std::string const &name = name_node[0]->as<token> ().string;
  std::string const &dir  = n.dir ()->as<token> ().string;

  if (state == S_PROGRAM)
    programs[dir].push_back (target (name, cond));
  else if (state == S_LIBRARY)
    libraries[dir].push_back (target (name, cond));

  cond.clear ();
}


void
emit_targets::visit (token &n)
{
}
