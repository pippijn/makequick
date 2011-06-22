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
  nodes::node_list *list;
  nodes::token *token;
}

%define api.pure
%locations
%debug
%error-verbose
%token-table
%glr-parser

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
%token KW_EXPORT		"export"
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
%token TK_COLON			":"
%token TK_WHITESPACE		"whitespace"

%token<token> TK_INT_LIB	"internal library"
%token<token> TK_EXT_LIB	"external library"
%token<token> TK_CODE		"shell code"
%token<token> TK_FILENAME	"filename"
%token<token> TK_IDENTIFIER	"identifier"
%token<token> TK_VAR		"variable"

%type<list> sources target_body target_definition
%type<list> program library template target_member target_members toplevel_declarations toplevel_declaration
%type<list> filename filenames.0 filenames.1 sources_member sources_members extra_dist nodist_sources
%type<list> rule rule_lines rule_line
%type<token> identifier filename_part rule_code_frag

%destructor { delete $$; } <*>

%%
/****************************************************************************
 *
 *	Toplevel declarations
 *
 ****************************************************************************/
document
	: toplevel_declarations
		{ self->doc = new generic_node ("document", $1); }
	;

toplevel_declarations
	: toplevel_declaration
		{ $$ = new generic_node ("toplevel_declarations", $1); }
	| toplevel_declarations toplevel_declaration
		{ ($$ = $1)->add ($2); }
	;

toplevel_declaration
	: program
		{ $$ = new generic_node ("toplevel_declaration", $1); }
	| library
		{ $$ = new generic_node ("toplevel_declaration", $1); }
	| template
		{ $$ = new generic_node ("toplevel_declaration", $1); }
	| extra_dist
		{ $$ = new generic_node ("toplevel_declaration", $1); }
	;


/****************************************************************************
 *
 *	Targets (program/library/template)
 *
 ****************************************************************************/
program
	: "program" target_definition
		{ $$ = new generic_node ("program", $2); }
	;

library
	: "library" target_definition
		{ $$ = new generic_node ("library", $2); }
	;

template
	: "template" target_definition
		{ $$ = new generic_node ("template", $2); }
	;

target_definition
	: identifier target_body
		{ $$ = new generic_node ("target_definition", $1, $2); }
	;

target_body
	: "{" target_members "}"
		{ $$ = new generic_node ("target_body", $2); }
	;

target_members
	: target_member
		{ $$ = new generic_node ("target_members", $1); }
	| target_members target_member
		{ ($$ = $1)->add ($2); }
	;

target_member
	: sources
	| nodist_sources
	| extra_dist
	| rule
	;

sources
	: "sources" "{" sources_members "}"
		{ $$ = new generic_node ("sources", $3); }
	;

nodist_sources
	: "nodist_sources" "{" sources_members "}"
		{ $$ = new generic_node ("nodist_sources", $3); }
	;

extra_dist
	: "extra_dist" "{" sources_members "}"
		{ $$ = new generic_node ("extra_dist", $3); }
	;

sources_members
	: sources_member
		{ $$ = new generic_node ("sources_members", $1); }
	| sources_members sources_member
		{ ($$ = $1)->add ($2); }
	;

sources_member
	: filename TK_WHITESPACE
	;


rule
	: filenames.1 ":" filenames.0 rule_begin rule_lines rule_end
		{ $$ = new generic_node ("rule", $1, $3, $5); }
	;

rule_begin
	: "{" { self->lex.push_state (yy::RULE_CODE); }
	;

rule_end
	: "}" { self->lex.pop_state (); }
	;

rule_lines
	: rule_line TK_WHITESPACE
		{ $$ = new generic_node ("rule_lines", $1); }
	| rule_lines rule_line TK_WHITESPACE
		{ ($$ = $1)->add ($2); }
	;

rule_line
	: rule_code_frag
		{ $$ = new generic_node ("rule_line", $1); }
	| rule_line rule_code_frag
		{ ($$ = $1)->add ($2); }
	;

rule_code_frag
	: TK_CODE
	| TK_VAR
	;

filenames.0
	:
		{ $$ = new generic_node ("filenames"); }
	| filenames.0 filename TK_WHITESPACE
		{ ($$ = $1)->add ($2); }
	;

filenames.1
	: filename
		{ $$ = new generic_node ("filenames", $1); }
	| filenames.1 TK_WHITESPACE filename
		{ ($$ = $1)->add ($3); }
	;


/****************************************************************************
 *
 *	Tokens
 *
 ****************************************************************************/
identifier: TK_IDENTIFIER;

filename
	: filename_part
		{ $$ = new generic_node ("filename", $1); }
	| filename filename_part
		{ ($$ = $1)->add ($2); }
	;

filename_part
	: TK_FILENAME
	;

%%

char const *
tokname (yySymbol yytoken)
{
  return yytname[yytoken];
}
