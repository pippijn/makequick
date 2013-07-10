#include "phase.h"

#include "util/make_filename.h"
#include "util/foreach.h"

#include <boost/lexical_cast.hpp>

struct clean_rules
  : visitor
{
  virtual void visit (t_toplevel_declarations &n);
  virtual void visit (t_rule &n);

  unsigned num;
  std::vector<std::string> cleans;

  std::string const &next_num ()
  {
    cleans.push_back ("clean-" + boost::lexical_cast<std::string> (num++));
    return cleans.back ();
  }

  clean_rules (annotation_map &annots)
    : num (1)
  {
  }
};

static phase<clean_rules> thisphase ("clean_rules", "resolve_sourcesref");


void
clean_rules::visit (t_toplevel_declarations &n)
{
  visitor::visit (n);

  t_filenames_ptr prereqs = new t_filenames (location::generated);
  foreach (std::string const &target, cleans)
    prereqs->add (make_filename (target));

  t_rule_ptr rule = new t_rule (location::generated,
                      (new t_filenames (location::generated))
                        ->add (make_filename ("clean-local")),
                      prereqs,
                      new t_rule_lines ());

  n.add (rule);
}

void
clean_rules::visit (t_rule &n)
{
  t_filename &target = n.target ()->as<t_filenames> ()[0]->as<t_filename> ();
  if (id (target[0]) == "clean")
    target.replace (0, new token (target[0]->loc, TK_FILENAME, next_num ()));
}
