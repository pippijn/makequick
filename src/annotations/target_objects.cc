#include "annotations/target_objects.h"

#include "foreach.h"

#include <iostream>

#include <boost/filesystem/path.hpp>

void
target_objects::print () const
{
  foreach (target_map::value_type const &target, targets)
    {
      std::cout << target.first << ":\n";
      foreach (fs::path const &obj, target.second)
        std::cout << '\t' << obj << '\n';
    }
}
