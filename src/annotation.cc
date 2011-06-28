#include "annotation.h"
#include "colours.h"

#include <stdexcept>

#include <boost/ptr_container/ptr_map.hpp>

struct annotation_map::pimpl
{
  typedef boost::ptr_map<std::string, annotation> map_type;

  map_type annots;
};

annotation_map::annotation_map ()
  : self (new pimpl)
{
}

annotation_map::~annotation_map ()
{
}


bool
annotation_map::has (std::string const &name)
{
  return self->annots.find (name) != self->annots.end ();
}

annotation_map::dynamic_caster
annotation_map::get (std::string const &name)
{
  pimpl::map_type::iterator found = self->annots.find (name);
  if (found == self->annots.end ())
    throw std::invalid_argument ("annotation " + C::filename (name) + " not found");
  return dynamic_caster (*found->second);
}

void
annotation_map::store (std::string const &name, annotation *annot)
{
  if (has (name))
    throw std::invalid_argument ("annotation " + C::filename (name) + " already present");
  self->annots.insert (name, std::auto_ptr<annotation> (annot));
}
