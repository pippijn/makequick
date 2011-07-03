#pragma once

#include "node.h"

#include <memory>

struct annotation_map;

struct phases
{
  static void run (std::string const &name, node_ptr doc, annotation_map &annots);
  static void run (node_ptr doc, annotation_map &annots);

private:
  virtual void run1 (node_ptr doc, annotation_map &annots) = 0;
  
  bool const autorun;

protected:
  phases (std::string const &name, bool autorun);
  ~phases ();

  void add_dependency (char const *dependency);

  struct pimpl;
  std::auto_ptr<pimpl> const self;
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

  phase (char const *name)
    : phases (name, true)
  {
  }

  phase (char const *name, char const *dep0)
    : phases (name, true)
  {
    add_dependency (dep0);
  }

  phase (char const *name, char const *dep0, char const *dep1)
    : phases (name, true)
  {
    add_dependency (dep0);
    add_dependency (dep1);
  }

  phase (char const *name, char const *dep0, char const *dep1, char const *dep2)
    : phases (name, true)
  {
    add_dependency (dep0);
    add_dependency (dep1);
    add_dependency (dep2);
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
