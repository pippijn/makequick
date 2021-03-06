#pragma once

#include "annotation.h"
#include "exception.h"

#include <boost/filesystem_fwd.hpp>
#include <boost/scoped_ptr.hpp>

struct error_log
  : annotation
{
  struct pimpl;
  boost::scoped_ptr<pimpl> const self;

  error_log ();
  ~error_log ();

  void add (semantic_error *e);

  template<typename Exception>
  void add (node_ptr const &node, std::string const &message)
  {
    add (new Exception (node, message));
  }

  template<typename Exception>
  void add (node_ptr const &node, std::string const &message, std::string const &note)
  {
    add (new Exception (node, message, note));
  }

  bool has_errors () const;
  bool has_diagnostics () const;
  void print (fs::path const &path, char const *base) const;
};
