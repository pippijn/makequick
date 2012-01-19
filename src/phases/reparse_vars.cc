#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "util/foreach.h"

struct reparse_vars
  : visitor
{
  void visit (t_vardecl_body &n);

  error_log &errors;

  reparse_vars (annotation_map &annots)
    : errors (annots.get ("errors"))
  {
  }
};

static phase<reparse_vars> thisphase ("reparse_vars", "flatten_vars");


void
reparse_vars::visit (t_vardecl_body &n)
{
  if (t_filename_ptr parent = n.parent ()->is<t_filename> ())
    {
      // if previous sibling is "{", this is a multi-rule
      //string_lexer lex (

      // if the expansion is the filename's only child, it may be a list
      // of filenames
    }
  else if (t_rule_line_ptr parent = n.parent ()->is<t_rule_line> ())
    {
      // lift all content into the parent
    }
}
