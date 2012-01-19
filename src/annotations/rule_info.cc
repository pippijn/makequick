#include "annotations/rule_info.h"

#include "util/foreach.h"

rule_info::rule const *
rule_info::find (std::string const &target) const
{
  foreach (rule const &r, rules)
    {
      if (r.target == target)
        return &r;
    }

  return NULL;
}
