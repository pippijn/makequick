#pragma once

#include "annotation.h"
#include "node_ptr.h"

#include <string>
#include <vector>

#include <boost/filesystem_fwd.hpp>
#include <boost/filesystem/path.hpp>

struct rule_info
  : annotation
{
  struct rule
  {
    std::string target;
    file_vec prereq;
    std::string stem;
    node_ptr code;
    node_ptr cond;

    void print () const;

    rule (std::string const &target, file_vec const &prereq, std::string const &stem, node_ptr const &code, node_ptr const &cond)
      : target (target)
      , prereq (prereq)
      , stem (stem)
      , code (code)
      , cond (cond)
    {
    }
  };

  struct rule_hash
  {
    size_t operator () (rule const &rule) const
    {
      return std::hash<std::string> () (rule.target);
    }
  };

  typedef std::unordered_set<rule, rule_hash> rule_set;

  rule const *find (std::string const &target) const;

  file_set files;
  rule_set rules;
};

namespace std {

  template<>
  struct hash<rule_info::rule>
    : rule_info::rule_hash
  {
  };

}

bool operator == (rule_info::rule const &a, rule_info::rule const &b);
