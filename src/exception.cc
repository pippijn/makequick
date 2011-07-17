#include "exception.h"

#include "colours.h"
#include "foreach.h"
#include "node.h"

#include <sstream>

#include <boost/filesystem/path.hpp>

enum severity_level
{
  WARNING,
  ERROR,
  NOTE
};

static std::string
severity (severity_level level)
{
  switch (level)
    {
    case WARNING:
      return C::yellow ("warning: ");
    case ERROR:
      return C::red ("error: ");
    case NOTE:
      return C::blue ("note: ");
    }
  assert (!"unreachable");
}

static std::ostream &
operator << (std::ostream &os, location const &loc)
{
  os << loc.file->native () << ":"
     << loc.first_line << ":"
     << loc.first_column << ": ";
  return os;
}


static std::string const
make_message (node_ptr node, std::string const &message, std::string const &note, bool error)
{
  std::ostringstream s;
  if (node)
    s << node->loc;
  else
    s << "(internal): ";
  s << severity (error ? ERROR : WARNING)
    << message;
  if (!note.empty ())
    s << "\n\t"
      //<< node->loc
      << severity (NOTE)
      << note;
  return s.str ();
}

semantic_error::semantic_error (node_ptr node, std::string const &message, bool error)
  : message (make_message (node, message, std::string (), error))
  , error (error)
{
}

semantic_error::semantic_error (node_ptr node, std::string const &message, std::string const &note, bool error)
  : message (make_message (node, message, note, error))
  , error (error)
{
}

char const *
semantic_error::what () const throw ()
{
  return message.c_str ();
}
