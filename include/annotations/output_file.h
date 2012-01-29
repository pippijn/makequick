#pragma once

#include "annotation.h"

#include <cstdio>

struct output_file
  : annotation
{
  FILE *Makefile;
  FILE *configure;
  char const *const base;

  output_file (char const *base, char const *Makefile, char const *configure);
  ~output_file ();

  void close ();
};
