#pragma once

#include <memory>

#include <boost/filesystem_fwd.hpp>
#include <boost/regex_fwd.hpp>
#include <boost/shared_ptr.hpp>

struct inference_engine
{
  struct promise
  {
    struct file_base
    {
      virtual void print () const = 0;
      virtual bool final () const = 0;
      virtual bool matches (fs::path const &file) const = 0;
      virtual std::string stem (fs::path const &file) const = 0;
    };
    
    template<typename T>
    struct file_t
      : file_base
    {
      T data;

      virtual void print () const;
      virtual bool final () const;
      virtual bool matches (fs::path const &file) const;
      virtual std::string stem (fs::path const &file) const;

      file_t (T const &data)
        : data (data)
      {
      }
    };

    boost::shared_ptr<file_base> file;

    file_base       *operator -> ()       { return file.get (); }
    file_base const *operator -> () const { return file.get (); }

    bool final () const { return file->final (); }
    bool matches (fs::path const &f) const { return file->matches (f); }

    template<typename T>
    promise (T const &file)
      : file (new file_t<T> (file))
    {
    }
  };

  struct pimpl;
  std::auto_ptr<pimpl> const self;

  inference_engine ();
  ~inference_engine ();

  void add_file (fs::path const &file);
  void add_rule (std::string const &target, std::vector<promise> const &prereqs);

  void add_rule (std::string const &target, promise const &dep0) { std::vector<promise> v; v.push_back (dep0); add_rule (target, v); }
  void add_rule (std::string const &target, promise const &dep0, promise const &dep1) { std::vector<promise> v; v.push_back (dep0); v.push_back (dep1); add_rule (target, v); }
  void add_rule (std::string const &target, promise const &dep0, promise const &dep1, promise const &dep2) { std::vector<promise> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); add_rule (target, v); }
  void add_rule (std::string const &target, promise const &dep0, promise const &dep1, promise const &dep2, promise const &dep3) { std::vector<promise> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); add_rule (target, v); }
  void add_rule (std::string const &target, promise const &dep0, promise const &dep1, promise const &dep2, promise const &dep3, promise const &dep4) { std::vector<promise> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); v.push_back (dep4); add_rule (target, v); }
  void add_rule (std::string const &target, promise const &dep0, promise const &dep1, promise const &dep2, promise const &dep3, promise const &dep4, promise const &dep5) { std::vector<promise> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); v.push_back (dep4); v.push_back (dep5); add_rule (target, v); }
  void add_rule (std::string const &target, promise const &dep0, promise const &dep1, promise const &dep2, promise const &dep3, promise const &dep4, promise const &dep5, promise const &dep6) { std::vector<promise> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); v.push_back (dep4); v.push_back (dep5); v.push_back (dep6); add_rule (target, v); }
  void add_rule (std::string const &target, promise const &dep0, promise const &dep1, promise const &dep2, promise const &dep3, promise const &dep4, promise const &dep5, promise const &dep6, promise const &dep7) { std::vector<promise> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); v.push_back (dep4); v.push_back (dep5); v.push_back (dep6); v.push_back (dep7); add_rule (target, v); }
  void add_rule (std::string const &target, promise const &dep0, promise const &dep1, promise const &dep2, promise const &dep3, promise const &dep4, promise const &dep5, promise const &dep6, promise const &dep7, promise const &dep8) { std::vector<promise> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); v.push_back (dep4); v.push_back (dep5); v.push_back (dep6); v.push_back (dep7); v.push_back (dep8); add_rule (target, v); }
  void add_rule (std::string const &target, promise const &dep0, promise const &dep1, promise const &dep2, promise const &dep3, promise const &dep4, promise const &dep5, promise const &dep6, promise const &dep7, promise const &dep8, promise const &dep9) { std::vector<promise> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); v.push_back (dep4); v.push_back (dep5); v.push_back (dep6); v.push_back (dep7); v.push_back (dep8); v.push_back (dep9); add_rule (target, v); }

  void infer ();
  void print () const;
};
