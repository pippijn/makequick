#include "string_lexer.h"
#include "yystate.h"

#include <cassert>
#include <climits>
#include <stdexcept>

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/path.hpp>

static fs::path const string_file = "<string>";

string_lexer::string_lexer (std::string const &s)
  : str (boost::algorithm::trim_copy (s))
{
  yy_scan_bytes (str.data (), str.length (), yyscanner);
  yyset_lineno (1, yyscanner);
}

string_lexer::~string_lexer ()
{
}

int
string_lexer::wrap ()
{
  return 1;
}

fs::path const *
string_lexer::current_file () const
{
  return &string_file;
}
