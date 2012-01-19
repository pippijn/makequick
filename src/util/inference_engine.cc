#include <cstdio>
#include <set>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/range.hpp>
#include <boost/spirit/home/phoenix.hpp>
#include <boost/thread.hpp>

#include "util/inference_engine.h"
#include "util/foreach.h"
#include "util/timer.h"


#define THREADED_PARTIALS 1


typedef inference_engine::rule rule;


void
inference_engine::add_file (fs::path const &file)
{
  info.files.push_back (file);
}

void
inference_engine::add_rule (std::string const &target,
                            std::vector<prerequisite> const &prereqs,
                            node_ptr const &code)
{
  rule r (target, prereqs, code);

  if (prereqs.empty ())
    {
      info.files.push_back (target);
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
  struct target_less
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

  struct stem_size_less
  {
    // a rule is smaller than another rule if it matched more precisely
    bool operator () (rule const &a, rule const &b)
    {
      return a.stem.size () < b.stem.size ();
    }
  };

  template<typename T>
  struct deref_less
  {
    bool operator () (T const *a, T const *b)
    {
      return *a < *b;
    }
  };

  typedef std::map<std::string, std::vector<rule> > partial_map;
  typedef std::vector<partial_map> partial_vec;
  typedef std::multiset<rule, target_less> rule_set;

  struct inferred
  {
    rule_set rules;
    std::vector<fs::path> files;
  };

  template<bool DoLock>
  struct partial_inference
  {
    void operator () (size_t from, size_t to)
    {
      typedef boost::iterator_range<std::vector<fs::path>::const_iterator> sub_range;

      foreach (fs::path const &f, sub_range (files.begin () + from, files.begin () + to))
        foreach (rule const &r, rules)
          {
            ptrdiff_t ridx = &r - &rules.front ();
            foreach (prerequisite const &p, r.prereqs)
              if (p->matches (f))
                {
                  // instantiate rule
                  rule instance = r;
                  instance.prereqs[&p - &r.prereqs.front ()] = f.native ();

                  if (DoLock) lock->lock ();
                  partials[ridx][p->stem (f)].push_back (instance);
                  if (DoLock) lock->unlock ();
                }
          }
    }

    partial_inference (std::vector<rule> const &rules,
                       std::vector<fs::path> const &files,
                       partial_vec &partials,
                       boost::mutex &lock)
      : rules (rules)
      , files (files)
      , partials (partials)
      , lock (&lock)
    {
    }

    partial_inference (std::vector<rule> const &rules,
                       std::vector<fs::path> const &files,
                       partial_vec &partials)
      : rules (rules)
      , files (files)
      , partials (partials)
    {
    }

    std::vector<rule> const &rules;
    std::vector<fs::path> const &files;
    partial_vec &partials;
    boost::mutex *lock;
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
   * <h3>Implementation</h3>
   * For each file, we check if a rule exists that depends on that file. If so,
   * we copy that rule and replace the matching prerequisite with the actual
   * file name. Each stem (\ref prerequisite::file_base::stem) has its own
   * instance set. The copied rule is added to this instance set. The \ref
   * resolve_partial function is responsible for merging these partial rules.
   */
  static void infer_partials (std::vector<rule> const &rules,
                              std::vector<fs::path> const &files,
                              partial_vec &partials)
  {
#if THREADED_PARTIALS
    unsigned long operations = files.size () * rules.size ();
    if (operations > 160000)
      {
        size_t partitions = std::min (10lu, operations / 80000);
#if 0
        printf ("doing %ld operations => splitting in %ld partitions\n", operations, partitions);
#endif

        boost::mutex lock;
        boost::ptr_vector<boost::thread> threads;
        // start workers
        for (size_t i = 0; i < partitions; i++)
          threads.push_back (
            new boost::thread (
              partial_inference<true> (rules, files, partials, lock),
              files.size () *  i      / partitions,
              files.size () * (i + 1) / partitions));

        // wait for each to finish
        foreach (boost::thread &thread, threads)
          thread.join ();
      }
    else
#endif
      {
        partial_inference<false> (rules, files, partials) (0, files.size ());
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
              throw std::runtime_error ("stem `" +
                                        rules.first +
                                        "' has no rules");

            rule &mainrule = rules.second[0];

            foreach (rule const &r, rules.second)
              foreach (prerequisite const &p, r.prereqs)
                if (p->final ())
                  mainrule.prereqs[&p - &*r.prereqs.begin ()] = p;

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
                                        mainrule.target))
                    != mainrule.prereqs.end ())
                  if (mainrule.stem == mainrule.target)
                    throw std::runtime_error ("target " +
                                              mainrule.target +
                                              " directly depends on itself");
                  else
                    continue;

                inferred.rules.insert (mainrule);
                inferred.files.push_back (mainrule.target);

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

  /** \brief Sort and unique a range.
   * 
   * This function ensures that the passed vector is sorted and has no
   * duplicate elements. It is used to compress the file list after inference,
   * as there may be many rules for a single file.
   */
  template<typename T>
  static void
  compact (std::vector<T> &range)
  {
    sort (range.begin (), range.end ());
    range.erase (unique (range.begin (), range.end ()), range.end ());
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
   * Finally, the list of inferred files is compacted (see \ref compact).
   *
   * \param baserules The rules added via \ref add_rule.
   * \param rules Where the inferred rules are stored.
   * \param files The input file list. Inferred files will be added to this
   * list.
   */
  static void infer (std::vector<rule> const &baserules,
                     std::vector<rule> &rules,
                     std::vector<fs::path> &files)
  {
    partial_vec partials (baserules.size ());

    inferred inferred;
    inferred.files = files;

    while (!inferred.files.empty ())
      {
        infer_partials (baserules, inferred.files, partials);
        inferred.files.clear ();
        resolve_partial (partials, inferred);
        files.insert (files.end (),
                      inferred.files.begin (),
                      inferred.files.end ());
      }

    // find most precisely matching rule
    {
      rule_set::iterator it = inferred.rules.begin ();
      rule_set::iterator et = inferred.rules.end ();
      while (it != et)
        {
          rule_set::iterator ubound = inferred.rules.upper_bound (*it);
          if (distance (it, ubound) > 1)
            {
              std::vector<rule> range (it, ubound);
              sort (range.begin (), range.end (), stem_size_less ());
              inferred.rules.erase (it, ubound);
              inferred.rules.insert (range.front ());
            }
          it = ubound;
        }
    }

    rules.insert (rules.end (),
                  inferred.rules.begin (),
                  inferred.rules.end ());

    compact (files);
  }
};

void
inference_engine::infer ()
{
#if 0
  timer T ("inference");
  printf ("running inference with %lu rules and %lu files\n", info.baserules.size (), info.files.size ());
#endif
  engine::infer (info.baserules, info.rules, info.files);
#if 0
  printf ("inferred %lu rules; we now have %lu files\n", info.rules.size (), info.files.size ());
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
}
