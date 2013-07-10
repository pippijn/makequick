#include "config.h"

#define USE_UNORDERED_SET 1

#include <cstdio>
#include <unordered_set>
#include <unordered_map>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/regex.hpp>
#include <boost/spirit/home/phoenix.hpp>

#include "util/inference_engine.h"
#include "util/foreach.h"
#include "util/timer.h"

#if HAVE_VALGRIND_CALLGRIND_H
#  include <valgrind/callgrind.h>
#endif


typedef inference_engine::rule rule;

void
inference_engine::add_file (fs::path const &file)
{
  info.files.insert (file);
}

void
inference_engine::add_rule (std::string const &target,
                            std::vector<prerequisite> const &prereqs,
                            node_ptr const &code,
                            node_ptr const &cond)
{
  rule r (target, prereqs, code, cond);

  if (prereqs.empty ())
    {
      info.files.insert (fs::path (target));
      info.rules.push_back (r);
    }
  else
    info.baserules.push_back (r);
}


/** \brief The actual inference algorithm implementation.
 *
 * This class implements the actual inference algorithm. It does not keep
 * state. The main entry point into the engine is \ref infer.
 */
class inference_engine::engine
{
  struct rule_less
  {
    bool operator () (rule const &a, rule const &b)
    {
      if (a.target < b.target)
        return true;
      if (a.target == b.target)
        return a.prereqs < b.prereqs;
      return false;
    }
  };

  struct rule_hash
  {
    size_t operator () (rule const &r) const
    {
      return ::hash (r.target);
    }
  };

  struct rule_eq
  {
    bool operator () (rule const &a, rule const &b) const
    {
      return a.target  == b.target
          && a.prereqs == b.prereqs;
    }
  };

  struct stem_size_less
  {
    // a rule is smaller than another rule if it matched more precisely
    bool operator () (rule const &a, rule const &b) const
    {
      return a.stem.size () < b.stem.size ();
    }
  };

  typedef std::unordered_map<std::string, std::vector<rule> > partial_map;
  typedef std::vector<partial_map> partial_vec;
  typedef std::unordered_multiset<rule, rule_hash, rule_eq> rule_set;

  struct inferred
  {
    rule_set rules;
    file_set files;
  };

  /** \brief Infer partial rules from patterns.
   *
   * The first step of the algorithm is to infer rules from pattern rules. It
   * is possible that a rule prerequisite does not exist, yet, but is shown to
   * be buildable, later on, after more rules have been inferred. The \ref
   * infer_partials function stores completely and also partially fulfilled
   * rules in the \p partials list.
   *
   * The complexity of this algorithm is \f$\Theta(f * r)\f$, as it always
   * processes the complete input. The actual time spent is, however, also
   * dependent on the number of prerequisites each rule has. This is considered
   * to be relatively constant, though.
   *
   * This function will start a maximum of 10 threads that can concurrently
   * handle rules.
   */
  static void infer_partials (std::vector<rule> const &rules,
                              file_set const &files,
                              partial_vec &partials)
  {
    size_t const operations  = files.size () * rules.size ();
    size_t const min_threads = 7;
    size_t const max_threads = 30;
    size_t const divisor     = 90000;
    size_t const partitions  = std::min (rules.size (),
                                 std::max (min_threads,
                                   std::min (max_threads, operations / divisor)));

#if 1
    timer T ("parallel infer_partials");
    printf ("%%%% doing %ld matchings => %ld parallel partitions\n",
            operations, partitions);
#pragma omp parallel for num_threads(partitions)
#else
    timer T ("serial infer_partials");
#endif
      for (size_t i = 0; i < rules.size (); i++)
        {
          rule const &r = rules[i];
          partial_map &map = partials[i];

          foreach (fs::path const &f, files)
            {
              foreach (prerequisite const &p, r.prereqs)
                {
                  std::string stem;
                  if (p->stem (f, stem))
                    {
                      // instantiate rule
                      rule instance = r;
                      instance.prereqs[&p - &r.prereqs.front ()] = f.native ();

                      map[stem].push_back (instance);
                    }
                }
            }
        }
  }

  /** \brief Merge partial rules into complete instances.
   *
   * A rule is complete if for each prerequisite of that rule, there is a
   * partial rule in the instance set, which has a realisation of that
   * prerequisite. It is impossible to have multiple partial rules with
   * different realisations of a single prerequisite, because partial rules are
   * mapped by stem. A different realisation of a single prerequisite always
   * has a different stem.
   *
   * Complete rules are added to the inferred rules list and the files these
   * rules can build are added to the files list.
   */
  static void resolve_partial (partial_vec &partials, inferred &inferred)
  {
    foreach (partial_map &map, partials)
      {
        std::vector<std::string> complete;
        foreach (partial_map::value_type &rules, map)
          {
            if (rules.second.empty ())
              throw std::runtime_error ("stem `" + rules.first + "' has no rules");

            rule &mainrule = rules.second[0];

            foreach (rule const &r, rules.second)
              foreach (prerequisite const &p, r.prereqs)
                if (p->final ())
                  mainrule.prereqs[&p - &r.prereqs.front ()] = p;

            using namespace boost::phoenix;
            using namespace boost::phoenix::arg_names;
            using boost::replace_first;

            if (std::find_if (mainrule.prereqs.begin (),
                              mainrule.prereqs.end (),
                              !bind (&prerequisite::final, arg1))
                == mainrule.prereqs.end ())
              {
                mainrule.stem = rules.first;
                replace_first (mainrule.target, "%", mainrule.stem);

                if (std::find_if (mainrule.prereqs.begin (),
                                  mainrule.prereqs.end (),
                                  bind (&prerequisite::matches, arg1,
                                        fs::path (mainrule.target)))
                    != mainrule.prereqs.end ())
                  if (mainrule.stem == mainrule.target)
                    throw std::runtime_error ("target " + mainrule.target + " directly depends on itself");
                  else
                    continue;

                inferred.rules.insert (mainrule);
                inferred.files.insert (fs::path (mainrule.target));

                complete.push_back (mainrule.stem);
              }
          }

        foreach (std::string const &stem, complete)
          map.erase (stem);
      }
  }

  static void print_partial (partial_vec const &partials)
  {
    foreach (partial_map const &map, partials)
      foreach (partial_map::value_type const &rules, map)
        {
          if (rules.second.empty ())
            throw std::runtime_error ("stem `" + rules.first + "' has no rules");

          printf ("stem: %s\n", rules.first.c_str ());

          foreach (rule const &r, rules.second)
            {
              printf ("  ");
              r.print ();
              printf ("\n");
            }
        }
  }

public:
  /** \brief Main entry point for the inference algorithm.
   *
   * This function accepts a list of initial rules and a list of initial files.
   * The files list is supplemented with inferred files. The initial rules list
   * is left alone and inferred rules are added to the output parameter \p rules.
   *
   * This is a multi-pass algorithm. In the first pass, we process the passed
   * file list. First, partial rules are inferred using \ref infer_partials.
   * These are then merged into complete rules by \ref resolve_partial. In
   * subsequent passes, the files inferred by the previous pass are processed
   * again. After each pass, the inferred files are added to the total file
   * list, but this list is not used in any pass except the first.
   *
   * \param baserules The rules added via \ref add_rule.
   * \param rules Where the inferred rules are stored.
   * \param files The input file list. Inferred files will be added to this
   * list.
   */
  static void infer (std::vector<rule> const &baserules,
                     std::vector<rule> &rules,
                     file_set &files)
  {
    partial_vec partials (baserules.size ());

    inferred inferred;
    inferred.files = files;

    while (!inferred.files.empty ())
      {
        infer_partials (baserules, inferred.files, partials);
        inferred.files.clear ();
        resolve_partial (partials, inferred);
        files.insert (inferred.files.begin (),
                      inferred.files.end ());
      }

    // find most precisely matching rule
    {
      rule_set::iterator it = inferred.rules.begin ();
      rule_set::iterator et = inferred.rules.end ();
      while (it != et)
        {
#if USE_UNORDERED_SET
          std::pair<rule_set::iterator, rule_set::iterator> range = inferred.rules.equal_range (*it);
          if (std::distance (range.first, range.second) > 1)
            {
              std::vector<rule> rules (range.first, range.second);
              sort (rules.begin (), rules.end (), stem_size_less ());
              inferred.rules.erase (range.first, range.second);
              inferred.rules.insert (rules.front ());
            }
          it = range.second;
#else
          rule_set::iterator ubound = inferred.rules.upper_bound (*it);
          if (std::distance (it, ubound) > 1)
            {
              std::vector<rule> range (it, ubound);
              sort (range.begin (), range.end (), stem_size_less ());
              inferred.rules.erase (it, ubound);
              inferred.rules.insert (range.front ());
            }
          it = ubound;
#endif
        }
    }

    rules.insert (rules.end (),
                  inferred.rules.begin (),
                  inferred.rules.end ());
  }
};

void
inference_engine::infer ()
{
#if 1
  size_t string_rules = 0;
  size_t pattern_rules = 0;
  size_t regex_rules = 0;
  foreach (rule const &r, info.baserules)
    {
      size_t *rules = &string_rules;
      foreach (prerequisite const &p, r.prereqs)
        {
          if (dynamic_cast<prerequisite::file_t<boost::regex> const *> (p.file.get ()))
            {
              rules = &regex_rules;
              break;
            }
          if (dynamic_cast<prerequisite::file_t<wildcard> const *> (p.file.get ()))
            {
              rules = &pattern_rules;
              break;
            }
        }
      ++*rules;
    }
  printf ("%%%% running inference with %lu pattern, %lu regex and %lu string rules with %lu files\n",
          pattern_rules, regex_rules, string_rules, info.files.size ());
#endif
#ifdef CALLGRIND_START_INSTRUMENTATION
  CALLGRIND_START_INSTRUMENTATION;
#endif
  engine::infer (info.baserules, info.rules, info.files);
#ifdef CALLGRIND_STOP_INSTRUMENTATION
  CALLGRIND_STOP_INSTRUMENTATION;
#endif
#if 1
  printf ("%%%% inferred %lu rules; we now have %lu (buildable or existing) files\n", info.rules.size (), info.files.size ());
#endif
}

void
inference_engine::print () const
{
  std::cout << "files:\n";
  foreach (fs::path const &f, info.files)
    std::cout << "  " << f << "\n";
  std::cout << "rule candidates:\n";
  foreach (rule const &r, info.baserules)
    {
      r.print ();
      std::cout << "\n";
    }
  std::cout << "complete rules:\n";
  foreach (rule const &r, info.rules)
    {
      r.print ();
      std::cout << "\n";
    }
}

inline void
rule::print () const
{
  std::cout << "  " << target << " <- ";
  foreach (prerequisite const &p, prereqs)
    {
      p->print ();
      if (&p != &prereqs.back ())
        std::cout << ", ";
    }
  if (!stem.empty ())
    std::cout << " ($* = " << stem << ")";
  if (code)
    std::cout << " {" << code << "}";
}
