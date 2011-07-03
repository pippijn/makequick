#include "annotations/build_dag.h"
#include "foreach.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/regex.hpp>

namespace annotations
{

struct rule
{
  typedef build_dag::promise promise;

  std::string target;
  std::vector<promise> prereqs;
  std::string stem;

  void print () const;

  rule (std::string const &target, std::vector<promise> const &prereqs)
    : target (target)
    , prereqs (prereqs)
  {
  }
};


struct build_dag::pimpl
{
  std::vector<fs::path> files;
  std::vector<rule> rules;
};

build_dag::build_dag ()
  : self (new pimpl)
{
}

build_dag::~build_dag ()
{
}


void
build_dag::add_file (fs::path const &file)
{
  self->files.push_back (file);
}

void
build_dag::add_rule (std::string const &target, std::vector<promise> const &prereqs)
{
  self->rules.push_back (rule (target, prereqs));
}


// check if any of the prereqs matches this file
static std::string
matches (fs::path const &f, std::vector<rule::promise> const &prereqs)
{
  foreach (rule::promise const &p, prereqs)
    if (p->matches (f))
      return p->stem (f);
  return "";
}

// check if all the prereqs match the same way
static bool
prereqs_exist (std::string const &stem,
               std::vector<fs::path> const &files,
               std::vector<rule::promise> const &prereqs)
{
  foreach (rule::promise const &p, prereqs)
    if (!p->exists (stem, files))
      return false;
  return true;
}

static bool
operator < (rule const &r1, rule const &r2)
{
  return r1.target < r2.target
      || r1.prereqs < r2.prereqs
      || r1.stem < r2.stem
      ;;
}

static bool
operator == (rule const &r1, rule const &r2)
{
  return r1.target == r2.target
      && r1.prereqs == r2.prereqs
      && r1.stem == r2.stem
      ;;
}

template<typename T>
static void
compact (std::vector<T> &vec)
{
  sort (vec.begin (), vec.end ());
  vec.erase (unique (vec.begin (), vec.end ()), vec.end ());
}

struct inference_engine
{
  typedef rule::promise promise;

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

              foreach (promise const &p, mainrule.prereqs)
                if (p->matches (mainrule.target))
                  throw std::runtime_error ("target " + mainrule.target + " directly depends on itself");

              inferred.rules.push_back (mainrule);
              inferred.files.push_back (mainrule.target);

              map.erase (mainrule.stem);
            }
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

  static void infer (std::vector<rule> &rules, std::vector<fs::path> &files)
  {
    partial_vec partials (rules.size ());

    inferred inferred;
    inferred.files = files;

    while (!inferred.files.empty ())
      {
        infer_partials (rules, inferred.files, partials);
        inferred.files.clear ();
        resolve_partial (partials, inferred);
        files.insert (files.end (), inferred.files.begin (), inferred.files.end ());
      }

    rules.insert (rules.end (), inferred.rules.begin (), inferred.rules.end ());

#if 0
    compact (files);
    compact (rules);
#endif
  }
};

void
build_dag::infer ()
{
  printf ("running inference with %lu rules and %lu files\n", self->rules.size (), self->files.size ());
  inference_engine::infer (self->rules, self->files);
  printf ("after inference, we have %lu rules and %lu files\n", self->rules.size (), self->files.size ());
}

void
build_dag::print () const
{
  std::cout << "files:\n";
  foreach (fs::path const &f, self->files)
    {
      std::cout << "  " << f << "\n";
    }
  std::cout << "rules:\n";
  foreach (rule const &r, self->rules)
    {
      r.print ();
      std::cout << "\n";
    }
}

void
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


template<>
std::string
build_dag::promise::file_t<std::string>::str () const
{
  return data;
}

template<>
void
build_dag::promise::file_t<std::string>::print () const
{
  std::cout << '"' << data << '"';
}

template<>
bool
build_dag::promise::file_t<std::string>::final () const
{
  return true;
}

template<>
bool
build_dag::promise::file_t<std::string>::matches (fs::path const &file) const
{
  return file == data;
}

template<>
bool
build_dag::promise::file_t<std::string>::exists (std::string const &stem, std::vector<fs::path> const &files) const
{
  foreach (fs::path const &f, files)
    if (f == data)
      return true;
  return false;
}

template<>
std::string
build_dag::promise::file_t<std::string>::stem (fs::path const &file) const
{
  return file == data ? file.native () : "";
}


template<>
std::string
build_dag::promise::file_t<boost::regex>::str () const
{
  return data.str ();
}

template<>
void
build_dag::promise::file_t<boost::regex>::print () const
{
  std::cout << '{' << data << '}';
}

template<>
bool
build_dag::promise::file_t<boost::regex>::final () const
{
  return false;
}

template<>
bool
build_dag::promise::file_t<boost::regex>::matches (fs::path const &file) const
{
  return regex_match (file.native (), data);
}

template<>
bool
build_dag::promise::file_t<boost::regex>::exists (std::string const &stem, std::vector<fs::path> const &files) const
{
  foreach (fs::path const &f, files)
    {
      boost::smatch matches;
      if (regex_match (f.native (), matches, data) && matches.str (1) == stem)
        return true;
    }
  return false;
}

template<>
std::string
build_dag::promise::file_t<boost::regex>::stem (fs::path const &file) const
{
  boost::smatch matches;
  regex_match (file.native (), matches, data);
  return matches.str (1);
}


}
