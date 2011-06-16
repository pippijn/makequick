%{
#include "parser.h"
#define YYSTYPE YYSTYPE
#define YYLEX_PARAM self->lex.lex

using namespace nodes;
%}

%union {
  nodes::node *node;
  nodes::token *token;
}

%define api.pure
%locations
%debug
%error-verbose
%token-table
%glr-parser

%parse-param { parser *self }
%lex-param { lexer *YYLEX_PARAM }

%token TK_EOF 0			"end of file"

%token<token> KW_PROJECT	"project"
%token<token> KW_IF		"if"
%token<token> KW_CPPFLAGS	"cppflags"
%token<token> KW_EXTRA_DIST	"extra_dist"
%token<token> KW_LIBRARY	"library"
%token<token> KW_LINK		"link"
%token<token> KW_CUSTOM		"custom"
%token<token> KW_BUILD		"build"
%token<token> KW_PROGRAM	"program"
%token<token> KW_RULE		"rule"
%token<token> KW_SOURCES	"sources"
%token<token> KW_NODIST_SOURCES	"nodist_sources"

%token<token> TK_LBRACE		"{"
%token<token> TK_RBRACE		"}"
%token<token> TK_ARROW		"->"

%token<token> TK_IDENTIFIER	"identifier"
%token<token> TK_STRING		"string literal"

%type<node> definition_list definition

%destructor { delete $$; } <*>

%%
document
	: definition_list		{ self->doc = $1; }
	;

definition_list
	: /* empty */			{ $$ = new definition_list; }
	| definition_list definition	{ dynamic_cast<definition_list *> ($$ = $1)->add ($2); }
	;

definition
	: TK_STRING			{ $$ = $1; }
	;

%%

char const *
yytokname (yySymbol yytoken)
{
  return yytokenName (yytoken);
}
