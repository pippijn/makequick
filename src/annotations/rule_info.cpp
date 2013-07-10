#include <algorithm>
#include <cstdio>

#include "annotations/rule_info.h"
#include "util/foreach.h"


bool
operator == (rule_info::rule const &a, rule_info::rule const &b)
{
  return a.target == b.target;
}


rule_info::rule const *
rule_info::find (std::string const &target) const
{
  rule_set::const_iterator found = rules.find (rule (target, std::vector<fs::path> (), std::string (), NULL, NULL));
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
