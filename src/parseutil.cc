#include "parseutil.h"
#include "parser.h"

#include "file_lexer.h"
#include "string_lexer.h"

#include <boost/filesystem/path.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/spirit/home/phoenix.hpp>


static node_ptr
parse (lexer &lex, annotations::error_log &errors)
{
  return parser (lex, errors) ();
}


static bool
is_rule_file (fs::path const &file)
{
  return file.extension () == ".mq";
}

node_ptr
parse_files (std::vector<fs::path> const &files, annotations::error_log &errors, int init, bool alternative)
{
  using namespace boost::adaptors;
  using boost::phoenix::arg_names::arg1;

  std::vector<fs::path const *> rule_files;
  boost::transform (files | filtered (is_rule_file), back_inserter (rule_files), &arg1);
  file_lexer lex (rule_files);
  lex.init (init, alternative);
  return parse (lex, errors);
}

node_ptr
parse_string (std::string const &s, annotations::error_log &errors, int init, bool alternative)
{
  string_lexer lex (s);
  lex.init (init, alternative);
  return parse (lex, errors);
}
