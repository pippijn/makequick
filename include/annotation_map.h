#pragma once

#include "annotation.h"

#include <iosfwd>
#include <boost/scoped_ptr.hpp>

struct annotation_map
{
  struct pimpl;
  boost::scoped_ptr<pimpl> self;

  annotation_map ();
  ~annotation_map ();

  struct dynamic_caster
  {
    annotation &annot;

    dynamic_caster (annotation &annot)
      : annot (annot)
    {
    }

    template<typename T>
    operator T & () { return dynamic_cast<T &> (annot); }
  };

  dynamic_caster get (std::string const &name);

  template<typename T>
  T &get (std::string const &name)
  { return get (name); }

  bool has (std::string const &name);

  void store (std::string const &name, annotation *annot);

  template<typename T>
  T &put (std::string const &name, T *annot)
  {
    store (name, annot);
    return *annot;
  }
};
