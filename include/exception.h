#pragma once

#include "node_ptr.h"

#include <exception>
#include <vector>

struct semantic_error
  : std::exception
{
  semantic_error (node_ptr node, std::string const &message, bool error = true);
  semantic_error (node_ptr node, std::string const &message, std::string const &note, bool error = true);

  ~semantic_error () throw ()
  {
  }

  char const *what () const throw ();

  std::string const message;
  bool const error;
};

struct syntax_error
  : semantic_error
{
  syntax_error (node_ptr node, std::string const &message, std::string const &note = "")
    : semantic_error (node, message, note)
  {
  }
};

struct warning
  : semantic_error
{
  warning (node_ptr node, std::string const &message, std::string const &note = "")
    : semantic_error (node, message, note, false)
  {
  }
};
