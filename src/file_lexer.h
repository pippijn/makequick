#pragma once

#include "lexer.h"

struct file_list;

struct file_lexer
  : lexer
{
  file_lexer (file_list const &files);
  ~file_lexer ();

  bool close_file ();

  virtual int wrap ();
  virtual fs::path const &current_file () const;

  file_list const &files;
  file_vec::const_iterator cur;
};
