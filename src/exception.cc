#include "exception.h"

#include "colours.h"
#include "foreach.h"
#include "node.h"

#include <sstream>

#include <boost/filesystem/path.hpp>

static std::string
severity (bool error)
{
  return error
       ? C::red ("error: ")
       : C::yellow ("warning: ")
       ;
}

static std::string const
make_message (semantic_error::node_vec const &nodes, std::string const &message, bool error)
{
  std::ostringstream s;
  foreach (node_ptr const &n, nodes)
    {
      if (&n != &nodes.back ())
        s << "from ";
      s << n->loc.file->native () << ":"
        << n->loc.first_line << ":"
        << n->loc.first_column;
      if (&n == &nodes.back ())
        s << ": " << severity (error) << message;
      else
        s << "\n";
    }
  return s.str ();
}

semantic_error::semantic_error (node_vec const &nodes, std::string const &message, bool error)
  : message (make_message (nodes, message, error))
  , error (error)
{
}

static std::string const
make_message (node_ptr node, std::string const &message, bool error)
{
  std::ostringstream s;
  s << node->loc.file->native ()<< ":"
    << node->loc.first_line << ":"
    << node->loc.first_column << ": "
    << severity (error)
    << message;
  return s.str ();
}

semantic_error::semantic_error (node_ptr node, std::string const &message, bool error)
  : message (make_message (node, message, error))
  , error (error)
{
}

char const *
semantic_error::what () const throw ()
{
  return message.c_str ();
}
