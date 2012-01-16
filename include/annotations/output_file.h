#pragma once

#include "annotation.h"

#include <cstdio>

struct output_file
  : annotation
{
  FILE *const Makefile;
  FILE *const configure;

  output_file (char const *Makefile, char const *configure);
  ~output_file ();
};
