%{
#include "parser.h"
#include "yystate.h"
#define YYSTYPE YYSTYPE

union YYSTYPE;
struct YYLTYPE;
static int
yylex (YYSTYPE *yylval, YYLTYPE *yylloc, parser *self)
{
  return self->lex.next (yylval, yylloc);
}

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
/*%glr-parser*/

%parse-param { parser *self }
%lex-param { parser *self }

%token TK_EOF 0			"end of file"

%token KW_IF			"if"
%token KW_ALIGNOF		"alignof"
%token KW_ARG_ENABLE		"arg_enable"
%token KW_ARG_WITH		"arg_with"
%token KW_C_BIGENDIAN		"c_bigendian"
%token KW_C_CHARSET		"c_charset"
%token KW_C_ENUM_FWDECL		"c_enum_fwdecl"
%token KW_C_FLOAT_FORMAT	"c_float_format"
%token KW_C_LATE_EXPANSION	"c_late_expansion"
%token KW_C_STDINT_H		"c_stdint_h"
%token KW_C_TOKEN_PASTE		"c_token_paste"
%token KW_C_TYPEOF		"c_typeof"
%token KW_CFLAGS		"cflags"
%token KW_CONFIG_HEADER		"config_header"
%token KW_CONTACT		"contact"
%token KW_CPPFLAGS		"cppflags"
%token KW_DEFINE		"define"
%token KW_EXCLUDE		"exclude"
%token KW_EXTRA_DIST		"extra_dist"
%token KW_FUNCTIONS		"functions"
%token KW_HEADER		"header"
%token KW_HEADERS		"headers"
%token KW_LIBRARY		"library"
%token KW_LINK			"link"
%token KW_NODIST_SOURCES	"nodist_sources"
%token KW_OPTIONS		"options"
%token KW_PROGRAM		"program"
%token KW_PROJECT		"project"
%token KW_SECTION		"section"
%token KW_SIZEOF		"sizeof"
%token KW_SOURCES		"sources"
%token KW_SYMBOL		"symbol"
%token KW_TEMPLATE		"template"
%token KW_VERSION		"version"

%token TK_LSQBRACK		"["
%token TK_RSQBRACK		"]"
%token TK_LBRACK		"("
%token TK_RBRACK		")"
%token TK_LBRACE		"{"
%token TK_RBRACE		"}"

%token TK_LIBRARY_REF		"library reference"
%token TK_FILENAME		"filename"
%token TK_IDENTIFIER		"identifier"
%token TK_INTEGER		"integer"
%token TK_KEYWORD		"keyword"
%token TK_OPERATOR		"operator"
%token TK_STRING		"string literal"

%destructor { delete $$; } <*>

%%
/****************************************************************************
 *
 *	Toplevel declarations
 *
 ****************************************************************************/
document
	: toplevel_declarations
	;

toplevel_declarations
	: toplevel_declaration
	| toplevel_declarations toplevel_declaration
	;

toplevel_declaration
	: program
	| library
	| template
	;


/****************************************************************************
 *
 *	Targets (program/library/template)
 *
 ****************************************************************************/
program
	: "program" target_definition
	;

library
	: "library" target_definition
	;

template
	: "template" target_definition
	;

target_definition
	: identifier target_body
	;

target_body
	: "{" target_members "}"
	;

target_members
	: target_member
	| target_members target_member
	;

target_member
	: sources
	| rule
	;

sources
	: "sources" sources_begin sources_members sources_end
	;

sources_begin
	: "{"
		{ self->lex.push_state (yy::SOURCES); }
	;

sources_end
	: "}"
		{ self->lex.pop_state (); }
	;

sources_members
	: sources_member
	| sources_members sources_member
	;

sources_member
	: filename
	;

rule
	: filename filename rule_begin rule_code rule_end
	;

rule_begin
	: "{"
		{ self->lex.push_state (yy::RULE_CODE); }
	;

rule_end
	: "}"
		{ self->lex.pop_state (); }
	;

rule_code
	: TK_STRING


/****************************************************************************
 *
 *	Tokens
 *
 ****************************************************************************/
/*string: TK_STRING;*/
identifier: TK_IDENTIFIER;
/*library_ref: TK_LIBRARY_REF;*/
filename: TK_FILENAME;
%%

char const *
tokname (yySymbol yytoken)
{
  return yytname[yytoken];
}
