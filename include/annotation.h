#pragma once

#include "colours.h"

struct annotation
{
  virtual ~annotation () { }
};

struct annotation_map
{
  typedef boost::ptr_map<std::string, annotation> map_type;

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

  dynamic_caster get (std::string const &name)
  {
    map_type::iterator found = annots.find (name);
    if (found == annots.end ())
      throw std::invalid_argument ("annotation " + C::filename (name) + " not found");
    return dynamic_caster (*found->second);
  }

  bool has (std::string const &name)
  {
    return annots.find (name) != annots.end ();
  }

  template<typename T>
  T &put (std::string const &name, T *annot)
  {
    if (has (name))
      throw std::invalid_argument ("annotation " + C::filename (name) + " already present");
    annots.insert (name, std::auto_ptr<annotation> (annot));
    return *annot;
  }

  map_type annots;
};
