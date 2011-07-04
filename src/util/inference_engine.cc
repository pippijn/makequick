#include "util/inference_engine.h"
#include "foreach.h"
#include "timing.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/regex.hpp>


typedef inference_engine::rule rule;


void
inference_engine::add_file (fs::path const &file)
{
  info.files.push_back (file);
}

void
inference_engine::add_rule (std::string const &target, std::vector<promise> const &prereqs, node_ptr const &code)
{
  rule r (target, prereqs, code);

  using namespace boost::phoenix;
  using namespace boost::phoenix::arg_names;

  if (std::find_if (prereqs.begin (),
                    prereqs.end (),
                    !bind (&promise::final, arg1))
      != prereqs.end ())
    info.baserules.push_back (r);
  else
    {
      info.files.push_back (target);
      info.rules.push_back (r);
    }
}


struct engine
{
  typedef inference_engine::promise promise;

  typedef std::map<std::string, std::vector<rule> > partial_map;
  typedef std::vector<partial_map> partial_vec;

  struct inferred
  {
    std::vector<rule> rules;
    std::vector<fs::path> files;
  };

  static void infer_partials (std::vector<rule> &rules, std::vector<fs::path> &files, partial_vec &partials)
  {
    foreach (fs::path const &f, files)
      foreach (rule const &r, rules)
        {
          int ridx = &r - &*rules.begin ();
          foreach (promise const &p, r.prereqs)
            if (p->matches (f))
              {
                // instantiate rule
                rule instance = r;
                instance.prereqs[&p - &*r.prereqs.begin ()] = f.native ();
                partials[ridx][p->stem (f)].push_back (instance);
              }
        }
  }

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
              foreach (promise const &p, r.prereqs)
                if (p->final ())
                  mainrule.prereqs[&p - &*r.prereqs.begin ()] = p;

            using namespace boost::phoenix;
            using namespace boost::phoenix::arg_names;
            using boost::algorithm::replace_first;

            if (std::find_if (mainrule.prereqs.begin (),
                              mainrule.prereqs.end (),
                              !bind (&promise::final, arg1))
                == mainrule.prereqs.end ())
              {
                mainrule.stem = rules.first;
                replace_first (mainrule.target, "%", mainrule.stem);

                if (std::find_if (mainrule.prereqs.begin (),
                                  mainrule.prereqs.end (),
                                  bind (&promise::matches, arg1, mainrule.target))
                    != mainrule.prereqs.end ())
                  throw std::runtime_error ("target " + mainrule.target + " directly depends on itself");

                inferred.rules.push_back (mainrule);
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

  template<typename T>
  static void
  compact (std::vector<T> &vec)
  {
    sort (vec.begin (), vec.end ());
    vec.erase (unique (vec.begin (), vec.end ()), vec.end ());
  }

  static void infer (std::vector<rule> &baserules, std::vector<rule> &rules, std::vector<fs::path> &files)
  {
    partial_vec partials (baserules.size ());

    inferred inferred;
    inferred.files = files;

    while (!inferred.files.empty ())
      {
        infer_partials (baserules, inferred.files, partials);
        inferred.files.clear ();
        resolve_partial (partials, inferred);
        files.insert (files.end (), inferred.files.begin (), inferred.files.end ());
      }

    rules.insert (rules.end (), inferred.rules.begin (), inferred.rules.end ());

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
  std::cout << "pattern rules:\n";
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
  foreach (promise const &p, prereqs)
    {
      p->print ();
      if (&p != &prereqs.back ())
        std::cout << ", ";
    }
  if (!stem.empty ())
    std::cout << " ($* = " << stem << ")";
}


template<typename T>
inference_engine::promise::file_t<T>::file_t (T const &data)
  : data (data)
{
}


template inference_engine::promise::file_t<std::string>::file_t (std::string const &data);

template<>
inline void
inference_engine::promise::file_t<std::string>::print () const
{
  std::cout << '"' << data << '"';
}

template<>
inline bool
inference_engine::promise::file_t<std::string>::final () const
{
  return true;
}

template<>
inline bool
inference_engine::promise::file_t<std::string>::matches (fs::path const &file) const
{
  return file == data;
}

template<>
inline std::string
inference_engine::promise::file_t<std::string>::stem (fs::path const &file) const
{
  return file == data ? file.native () : "";
}


template inference_engine::promise::file_t<boost::regex>::file_t (boost::regex const &data);

template<>
inline void
inference_engine::promise::file_t<boost::regex>::print () const
{
  std::cout << '{' << data << '}';
}

template<>
inline bool
inference_engine::promise::file_t<boost::regex>::final () const
{
  return false;
}

template<>
inline bool
inference_engine::promise::file_t<boost::regex>::matches (fs::path const &file) const
{
  return regex_match (file.native (), data);
}

template<>
inline std::string
inference_engine::promise::file_t<boost::regex>::stem (fs::path const &file) const
{
  boost::smatch matches;
  regex_match (file.native (), matches, data);
  return matches.str (1);
}
