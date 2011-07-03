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
  std::vector<promise> dependencies;

  rule (std::string const &target, std::vector<promise> const &dependencies)
    : target (target)
    , dependencies (dependencies)
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
build_dag::add_rule (std::string const &target, std::vector<promise> const &dependencies)
{
  self->rules.push_back (rule (target, dependencies));
}


// check if any of the dependencies matches this file
static std::string
matches (fs::path const &f, std::vector<rule::promise> const &dependencies)
{
  foreach (rule::promise const &p, dependencies)
    if (p->matches (f))
      return p->match (f);
  return "";
}

// check if all the dependencies match the same way
static bool
prereqs_exist (std::string const &stem,
               std::vector<fs::path> const &files,
               std::vector<rule::promise> const &dependencies)
{
  foreach (rule::promise const &p, dependencies)
    if (!p->exists (stem, files))
      return false;
  return true;
}

static void
compact (std::vector<fs::path> &files)
{
  sort (files.begin (), files.end ());
  std::vector<fs::path>::iterator end = unique (files.begin (), files.end ());
  files.erase (end, files.end ());
}

void
build_dag::infer ()
{
#define VERBOSE 1
  foreach (rule const &r, self->rules)
    {
#if VERBOSE
      printf ("target %s\n", r.target.c_str ());
#endif
      std::vector<fs::path> files;
      foreach (fs::path const &f, self->files)
        {
          std::string const &stem = matches (f, r.dependencies);
          // check if one dependency matches
          if (!stem.empty ())
            {
#if VERBOSE
              printf ("  one dependency matches: %s ($* = %s)\n", f.c_str (), stem.c_str ());
#endif
              // check if all dependencies match
              if (prereqs_exist (stem, self->files, r.dependencies))
                {
#if VERBOSE
                  puts ("  all dependencies match");
#endif
                  files.push_back (boost::algorithm::replace_first_copy (r.target, "%", stem));
                }
            }
        }
      self->files.insert (self->files.end (), files.begin (), files.end ());
      compact (self->files);
    }
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
      std::cout << "  " << r.target << " <- ";
      foreach (promise const &p, r.dependencies)
        {
          p->print ();
          if (&p != &r.dependencies.back ())
            std::cout << ", ";
        }
      std::cout << "\n";
    }
}


template<>
void
build_dag::promise::file_t<std::string>::print () const
{
  std::cout << '"' << data << '"';
}

template<>
bool
build_dag::promise::file_t<std::string>::matches (fs::path const &file) const
{
  return file.native () == data;
}

template<>
bool
build_dag::promise::file_t<std::string>::exists (std::string const &stem, std::vector<fs::path> const &files) const
{
  foreach (fs::path const &f, files)
    if (f.native () == data)
      return true;
  return false;
}

template<>
std::string
build_dag::promise::file_t<std::string>::match (fs::path const &file) const
{
  return file.native () == data ? file.native () : "";
}


template<>
void
build_dag::promise::file_t<boost::regex>::print () const
{
  std::cout << '/' << data << '/';
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
build_dag::promise::file_t<boost::regex>::match (fs::path const &file) const
{
  boost::smatch matches;
  regex_match (file.native (), matches, data);
  return matches.str (1);
}


}
