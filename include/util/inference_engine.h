#pragma once

#include "node_ptr.h"

#include <memory>

#include <boost/filesystem_fwd.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>

/** \brief Pattern rule inference engine.
 *
 * This file implements the inference engine. Inference rules are modelled
 * after GNU Make pattern rules. This engine uses regular expressions to
 * implement pattern matching.
 *
 * The engine takes files and rules as input and produces a possibly larger
 * list of files and a list of inferred rules.
 *
 * Using the engine is very simple:
 * - Add files using \c add_file and rules using \c add_rule.
 * - Run inference.
 *
 * The engine operates purely on the knowledge provided through these two
 * functions. It does not perform any file system operations. It does not care
 * if a file actually exists or not.
 *
 * We run the engine multiple times. Each subsequent run considers only the
 * files generated by the previous run. This way, the algorithm complexity is
 * minimised to \f$O(r * f)\f$ where \f$r\f$ is the number of rules (pattern
 * rules and normal rules) and \f$f\f$ .
 */
struct inference_engine
{
  struct engine;

  /** \brief A requirement for the rule to match.
   */
  struct prerequisite
  {
    /** \brief Interface implemented by actual file representations.
     */
    struct file_base
    {
      virtual void print () const = 0;
      virtual bool final () const = 0;
      virtual bool matches (fs::path const &file) const = 0;
      /** \brief The match stem.
       *
       * For pattern rules, it is the matched "%", for plain files, it is the
       * file name itself.
       */
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

      file_t (T const &data);
    };

    boost::shared_ptr<file_base> file;

    file_base       *operator -> ()       { return file.get (); }
    file_base const *operator -> () const { return file.get (); }

    bool final () const { return file->final (); }
    bool matches (fs::path const &f) const { return file->matches (f); }

    std::string const &str () const { return dynamic_cast<file_t<std::string> &> (*file).data; }

    template<typename T>
    prerequisite (T const &file)
      : file (new file_t<T> (file))
    {
    }
  };

  struct rule
  {
    std::string target;
    std::vector<prerequisite> prereqs;
    std::string stem;
    node_ptr code;

    void print () const;

    rule (std::string const &target, std::vector<prerequisite> const &prereqs, node_ptr const &code)
      : target (target)
      , prereqs (prereqs)
      , code (code)
    {
    }
  };

  struct information
  {
    std::vector<fs::path> files;
    std::vector<rule> baserules;
    std::vector<rule> rules;
  };
  information info;

  /// Add a file to the initially assumed file list.
  void add_file (fs::path const &file);

  /// Add a rule to the initial rule list.
  void add_rule (std::string const &target, std::vector<prerequisite> const &prereqs, node_ptr const &code);

  void add_rule (std::string const &target, prerequisite const &dep0) { std::vector<prerequisite> v; v.push_back (dep0); add_rule (target, v, 0); }
  void add_rule (std::string const &target, prerequisite const &dep0, prerequisite const &dep1) { std::vector<prerequisite> v; v.push_back (dep0); v.push_back (dep1); add_rule (target, v, 0); }
  void add_rule (std::string const &target, prerequisite const &dep0, prerequisite const &dep1, prerequisite const &dep2) { std::vector<prerequisite> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); add_rule (target, v, 0); }
  void add_rule (std::string const &target, prerequisite const &dep0, prerequisite const &dep1, prerequisite const &dep2, prerequisite const &dep3) { std::vector<prerequisite> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); add_rule (target, v, 0); }
  void add_rule (std::string const &target, prerequisite const &dep0, prerequisite const &dep1, prerequisite const &dep2, prerequisite const &dep3, prerequisite const &dep4) { std::vector<prerequisite> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); v.push_back (dep4); add_rule (target, v, 0); }
  void add_rule (std::string const &target, prerequisite const &dep0, prerequisite const &dep1, prerequisite const &dep2, prerequisite const &dep3, prerequisite const &dep4, prerequisite const &dep5) { std::vector<prerequisite> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); v.push_back (dep4); v.push_back (dep5); add_rule (target, v, 0); }
  void add_rule (std::string const &target, prerequisite const &dep0, prerequisite const &dep1, prerequisite const &dep2, prerequisite const &dep3, prerequisite const &dep4, prerequisite const &dep5, prerequisite const &dep6) { std::vector<prerequisite> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); v.push_back (dep4); v.push_back (dep5); v.push_back (dep6); add_rule (target, v, 0); }
  void add_rule (std::string const &target, prerequisite const &dep0, prerequisite const &dep1, prerequisite const &dep2, prerequisite const &dep3, prerequisite const &dep4, prerequisite const &dep5, prerequisite const &dep6, prerequisite const &dep7) { std::vector<prerequisite> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); v.push_back (dep4); v.push_back (dep5); v.push_back (dep6); v.push_back (dep7); add_rule (target, v, 0); }
  void add_rule (std::string const &target, prerequisite const &dep0, prerequisite const &dep1, prerequisite const &dep2, prerequisite const &dep3, prerequisite const &dep4, prerequisite const &dep5, prerequisite const &dep6, prerequisite const &dep7, prerequisite const &dep8) { std::vector<prerequisite> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); v.push_back (dep4); v.push_back (dep5); v.push_back (dep6); v.push_back (dep7); v.push_back (dep8); add_rule (target, v, 0); }
  void add_rule (std::string const &target, prerequisite const &dep0, prerequisite const &dep1, prerequisite const &dep2, prerequisite const &dep3, prerequisite const &dep4, prerequisite const &dep5, prerequisite const &dep6, prerequisite const &dep7, prerequisite const &dep8, prerequisite const &dep9) { std::vector<prerequisite> v; v.push_back (dep0); v.push_back (dep1); v.push_back (dep2); v.push_back (dep3); v.push_back (dep4); v.push_back (dep5); v.push_back (dep6); v.push_back (dep7); v.push_back (dep8); v.push_back (dep9); add_rule (target, v, 0); }

  /// Run the inference algorithm on the input set.
  void infer ();
  /// Print the knowledge database.
  void print () const;
};
