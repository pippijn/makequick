#pragma once

#include "node.h"
#include "annotation.h"

#include <cstdarg>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

struct phases
{
  static void run (std::string const &name, node_ptr doc, annotation_map &annots);
  static void run (node_ptr doc, annotation_map &annots);

private:
  virtual void run1 (node_ptr doc, annotation_map &annots) = 0;
  
  bool const autorun;

protected:
  phases (std::string const &name, bool autorun);

  std::tr1::unordered_set<std::string> dependencies;
};

struct noauto_tag { };
#define noauto noauto_tag ()

template<typename Visitor>
struct phase
  : private phases
{
  friend struct phases;

  phase (char const *name, noauto_tag)
    : phases (name, false)
  {
  }

  phase (char const *name, ...)
    : phases (name, true)
  {
    va_list ap;
    va_start (ap, name);
    while (char const *dependency = va_arg (ap, char const *))
      dependencies.insert (dependency);
    va_end (ap);
  }

  void run1 (node_ptr doc, annotation_map &annots)
  {
    Visitor v (annots);
    this->annots = &annots;
    doc->accept (v);
    this->annots = NULL;
  }

  annotation_map *annots;
};
