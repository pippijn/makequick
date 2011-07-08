#include <fstream>
#include <set>

#include <boost/archive/detail/basic_iarchive.hpp>
#include <boost/archive/detail/basic_oarchive.hpp>

#include <boost/serialization/vector.hpp>

#include <boost/implicit_cast.hpp>

#include <boost/utility/enable_if.hpp>

#include <boost/filesystem/path.hpp>

#include "foreach.h"
#include "node.h"

using namespace nodes;
using namespace tokens;

struct s11n_files
{
  std::set<fs::path const *> files;

  void add (boost::archive::detail::basic_oarchive const &, fs::path const *file) { }
  void add (boost::archive::detail::basic_iarchive const &, fs::path const *file)
  { files.insert (file); }

  ~s11n_files ()
  {
    foreach (fs::path const *file, files)
      delete file;
  }
} files;

namespace boost
{
  namespace serialization
  {
    template<typename Archive>
    static void
    serialize (Archive &ar, fs::path &file, unsigned int version)
    {
      std::string native = file.native ();
      ar & make_nvp ("native", native);
      file = native;
    }

    template<typename Archive>
    static void
    serialize (Archive &ar, location &n, unsigned int version)
    {
      ar & make_nvp ("file", n.file);
      ar & make_nvp ("first_line", n.first_line);
      ar & make_nvp ("first_column", n.first_column);
      ar & make_nvp ("last_line", n.last_line);
      ar & make_nvp ("last_column", n.last_column);

      files.add (ar, n.file);
    }

    template<typename Archive>
    static void
    serialize (Archive &ar, node &n, unsigned int version)
    {
      ar & make_nvp ("loc", n.loc);
    }

    template<typename Archive>
    static void
    serialize (Archive &ar, token &n, unsigned int version)
    {
      serialize (ar, implicit_cast<node &> (n), version);
      ar & make_nvp ("tok", n.tok);
      ar & make_nvp ("string", n.mutable_string);
    }

    template<typename Archive>
    static void
    serialize (Archive &ar, generic_node &n, unsigned int version)
    {
      serialize (ar, implicit_cast<node &> (n), version);
      ar & make_nvp ("type", n.type);
      ar & make_nvp ("list", n.list);
    }

#include "s11n_funcs.h"

    template<typename Archive>
    static void
    serialize (Archive &ar, node_ptr &n, unsigned int version)
    {
      void_cast_register<token, node> ();
      void_cast_register<generic_node, node> ();
#include "s11n_casts.h"

      ar.template register_type<token> ();
#include "s11n_types.h"

      node *p = n.get ();
      ar & make_nvp ("px", p);
      n.reset (p);
    }
  }
}

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

template<s11n_format Format> struct suffix;
template<> struct suffix<s11n_binary> { static std::string value () { return ".bin"; } };
template<> struct suffix<s11n_text  > { static std::string value () { return ".txt"; } };
template<> struct suffix<s11n_xml   > { static std::string value () { return ".xml"; } };

template<s11n_format Format> struct iarchive;
template<> struct iarchive<s11n_binary> { typedef boost::archive::binary_iarchive type; };
template<> struct iarchive<s11n_text  > { typedef boost::archive::text_iarchive   type; };
template<> struct iarchive<s11n_xml   > { typedef boost::archive::xml_iarchive    type; };

template<s11n_format Format> struct oarchive;
template<> struct oarchive<s11n_binary> { typedef boost::archive::binary_oarchive type; };
template<> struct oarchive<s11n_text  > { typedef boost::archive::text_oarchive   type; };
template<> struct oarchive<s11n_xml   > { typedef boost::archive::xml_oarchive    type; };

template<s11n_format Format>
struct serialiser
{
  static node_ptr load (char const *file)
  {
    node_ptr n;

    std::ifstream ifs ((file + suffix<Format>::value ()).c_str ());
    typename iarchive<Format>::type ia (ifs);
    ia >> boost::serialization::make_nvp ("node", n);

    return n;
  }

  static void store (char const *file, node_ptr const &n)
  {
    std::ofstream ofs ((file + suffix<Format>::value ()).c_str ());
    typename oarchive<Format>::type oa (ofs);
    oa << boost::serialization::make_nvp ("node", n);
  }
};


node_ptr
node::load (char const *file, s11n_format format)
{
  switch (format)
    {
    case s11n_binary: return serialiser<s11n_binary>::load (file);
    case s11n_text  : return serialiser<s11n_text  >::load (file);
    case s11n_xml   : return serialiser<s11n_xml   >::load (file);
    }
  throw format;
}

void
node::store (char const *file, node_ptr const &n, s11n_format format)
{
  switch (format)
    {
    case s11n_binary: return serialiser<s11n_binary>::store (file, n);
    case s11n_text  : return serialiser<s11n_text  >::store (file, n);
    case s11n_xml   : return serialiser<s11n_xml   >::store (file, n);
    }
  throw format;
}
