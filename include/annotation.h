#pragma once

#include <iosfwd>
#include <memory>

struct annotation
{
  virtual ~annotation () { }
};

struct annotation_map
{
  struct pimpl;
  std::auto_ptr<pimpl> self;

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

  bool has (std::string const &name);

  void store (std::string const &name, annotation *annot);

  template<typename T>
  T &put (std::string const &name, T *annot)
  {
    store (name, annot);
    return *annot;
  }
};
