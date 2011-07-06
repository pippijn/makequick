#pragma once

#include "lexer.h"

struct file_lexer
  : lexer
{
  typedef std::vector<fs::path const *> file_vec;

  file_lexer (file_vec const &files);
  ~file_lexer ();

  bool close_file ();

  virtual int wrap ();
  virtual fs::path const *current_file () const;

  file_vec::const_iterator it;
  file_vec::const_iterator et;
};
