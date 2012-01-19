#pragma once

#include "fwd.hpp"

#include <boost/filesystem/path.hpp>

namespace fs
{
  struct path
  {
    path ()
    {
    }

    explicit path (boost::filesystem::path const &data)
      : m (data)
    {
    }

    boost::filesystem::path const &data () const { return m.data; }

    struct m
    {
      boost::filesystem::path data;
#if STORE_PATH_HASH
      size_t hash;
#endif

      m ();
      m (boost::filesystem::path data);
    } m;
  };
}
