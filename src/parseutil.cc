#include "parseutil.h"
#include "parser.h"

#include "file_lexer.h"
#include "string_lexer.h"

#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/algorithm/transform.hpp>


static node_ptr
parse (lexer &lex)
{
  return parser (lex) ();
}


static bool
is_rule_file (fs::path const &file)
{
  return file.extension () == ".mq";
}

node_ptr
parse_files (std::vector<fs::path> const &files, int init, bool alternative)
{
  using namespace boost::adaptors;
  using boost::phoenix::arg_names::arg1;

  std::vector<fs::path const *> rule_files;
  boost::transform (files | filtered (is_rule_file), back_inserter (rule_files), &arg1);
  file_lexer lex (rule_files);
  lex.init (init, alternative);
  return parse (lex);
}

node_ptr
parse_string (std::string const &s, int init, bool alternative)
{
  string_lexer lex (s);
  lex.init (init, alternative);
  return parse (lex);
}
