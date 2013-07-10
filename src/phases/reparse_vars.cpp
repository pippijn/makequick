#include "phase.h"

#include "annotations/error_log.h"
#include "annotations/symbol_table.h"
#include "util/extract_string.h"
#include "util/foreach.h"

#include "parser.h"
#include "string_lexer.h"
#include "yystate.h"

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
      if (n.has_prev_sibling ())
        if (token_ptr tok = n.prev_sibling ()->is<token> ())
          {
            if (tok->tok == TK_FN_LBRACE)
              {
                string_lexer lex (extract_string (n));
                lex.push_state (yy::FILENAME);
                lex.push_state (yy::MULTIFILE);
                lex.init_token (R_FILENAME);
                if (node_ptr result = parser (lex, errors) ())
                  parent->replace (n, result->as<t_filename> ().list);
              }
          }

      // if the expansion is the filename's only child, it may be a list
      // of filenames, otherwise it's part of another filename
      if (n.has_sibling ())
        {
          string_lexer lex (extract_string (n));
          lex.push_state (yy::FILENAME);
          lex.init_token (R_FILENAME);
          if (node_ptr result = parser (lex, errors) ())
            parent->replace (n, result->as<t_filename> ().list);
        }
      else if (n.parent () && (parent = n.parent ()->is<t_filename> ()))
        {
          string_lexer lex (extract_string (n));
          lex.push_state (yy::FILENAME);
          lex.init_token (TK_WHITESPACE);
          lex.init_token (R_SOURCES);
          if (node_ptr result = parser (lex, errors) ())
            parent->parent ()->replace (*parent, result->as<t_sources_members> ().list);
        }
    }
  else if (t_rule_line_ptr parent = n.parent ()->is<t_rule_line> ())
    {
      // lift all content into the parent
      parent->replace (n, n.list);
    }
}
