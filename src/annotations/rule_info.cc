#include <algorithm>
#include <cstdio>

#include "annotations/rule_info.h"
#include "util/foreach.h"

struct target_eq
{
  target_eq (std::string const &target)
    : target (target)
  {
  }

  bool operator () (rule_info::rule const &a)
  {
    return a.target == target;
  }

  std::string const &target;
};

rule_info::rule const *
rule_info::find (std::string const &target) const
{
  std::vector<rule>::const_iterator found = find_if (rules.begin (), rules.end (), target_eq (target));
  if (found != rules.end ())
    return &*found;

  return NULL;
}


void
rule_info::rule::print () const
{
  printf ("%s:", target.c_str ());
  foreach (fs::path const &p, prereq)
    printf (" %s", p.c_str ());
  printf (" ($* = %s) { %p }\n", stem.c_str (), code.get ());
}
