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
  tokens::token *token;
}

%define api.pure
%locations
%debug
%error-verbose
%token-table
%glr-parser

%parse-param	{ parser *self }
%lex-param	{ parser *self }

%token TK_EOF 0				"end of file"

%token<token> KW_IF			"if"
%token<token> KW_ALIGNOF		"alignof"
%token<token> KW_ARG_ENABLE		"arg_enable"
%token<token> KW_ARG_WITH		"arg_with"
%token<token> KW_C_BIGENDIAN		"c_bigendian"
%token<token> KW_C_CHARSET		"c_charset"
%token<token> KW_C_ENUM_FWDECL		"c_enum_fwdecl"
%token<token> KW_C_FLOAT_FORMAT		"c_float_format"
%token<token> KW_C_LATE_EXPANSION	"c_late_expansion"
%token<token> KW_C_STDINT_H		"c_stdint_h"
%token<token> KW_C_STMT_EXPRS		"c_stmt_exprs"
%token<token> KW_C_TOKEN_PASTE		"c_token_paste"
%token<token> KW_C_TYPEOF		"c_typeof"
%token<token> KW_CFLAGS			"cflags"
%token<token> KW_CONFIG_HEADER		"config_header:"
%token<token> KW_CONTACT		"contact:"
%token<token> KW_CPPFLAGS		"cppflags"
%token<token> KW_DEFINE			"define"
%token<token> KW_ERROR			"error"
%token<token> KW_EXCLUDE		"exclude"
%token<token> KW_EXTRA_DIST		"extra_dist"
%token<token> KW_FUNCTIONS		"functions"
%token<token> KW_GLOBAL			"global"
%token<token> KW_HEADER			"header:"
%token<token> KW_HEADERS		"headers"
%token<token> KW_LIBRARY		"library"
%token<token> KW_LINK			"link"
%token<token> KW_NODIST_SOURCES		"nodist_sources"
%token<token> KW_NOTFOUND		"notfound:"
%token<token> KW_OPTIONS		"options"
%token<token> KW_PROGRAM		"program"
%token<token> KW_PROJECT		"project"
%token<token> KW_SECTION		"section"
%token<token> KW_SIZEOF			"sizeof"
%token<token> KW_SOURCES		"sources"
%token<token> KW_SYMBOL			"symbol:"
%token<token> KW_TEMPLATE		"template"
%token<token> KW_VERSION		"version:"

%token<token> TK_LSQBRACK		"["
%token<token> TK_RSQBRACK		"]"
%token<token> TK_LBRACK			"("
%token<token> TK_RBRACK			")"
%token<token> TK_LBRACE			"{"
%token<token> TK_RBRACE			"}"
%token<token> TK_COLON			":"
%token<token> TK_EQUALS			"="
%token<token> TK_ARROW			"->"
%token<token> TK_DARROW			"=>"
%token<token> TK_WHITESPACE		"whitespace"

%token<token> TK_EXT_LIB		"external library"
%token<token> TK_INT_LIB		"internal library"

%token<token> TK_CODE			"shell code"
%token<token> TK_FILENAME		"filename"
%token<token> TK_FLAG			"tool flag"
%token<token> TK_IDENTIFIER		"identifier"
%token<token> TK_STRING			"string literal"
%token<token> TK_VAR			"variable"

%type<list> sources target_body target_definition
%type<list> program library template target_member target_members toplevel_declarations toplevel_declaration
%type<list> filename filenames.0 filenames.1 sources_member sources_members extra_dist nodist_sources
%type<list> rule rule_lines rule_line link link_body vardecl vardecl_body
%type<list> description inheritance.opt inheritance if.opt if destination.opt destination
%type<list> tool_flags flags identifiers
%type<list> check_alignof check_cflags check_functions check_headers check_library check_sizeof
%type<list> check_library_notfound.opt define arg_enable arg_with arg_with_options arg_with_choices
%type<list> arg_with_choice project project_members project_member section section_members section_member
%type<token> identifier filename_part code_frag link_item flag_keyword string.opt identifier.opt ac_checks
%type<token> arg_default

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
	: project
		{ $$ = new generic_node ("toplevel_declaration", $1); }
	| program
		{ $$ = new generic_node ("toplevel_declaration", $1); }
	| library
		{ $$ = new generic_node ("toplevel_declaration", $1); }
	| template
		{ $$ = new generic_node ("toplevel_declaration", $1); }
	| extra_dist
		{ $$ = new generic_node ("toplevel_declaration", $1); }
	| "global" "{" rule "}"
		{ $$ = new generic_node ("toplevel_declaration", $3); delete $1; delete $2; delete $4; }
	;


/****************************************************************************
 *
 *	Project configuration
 *
 ****************************************************************************/
project
	: "project" TK_STRING "{" project_members "}"
		{ $$ = new generic_node ("project", $2, $4); delete $1; delete $3; delete $5; }
	;

project_members
	: project_member
		{ $$ = new generic_node ("project_members", $1); }
	| project_members project_member
		{ ($$ = $1)->add ($2); }
	;

project_member
	: "version:" TK_STRING
		{ $$ = new generic_node ("project_member", $2); delete $1; }
	| "contact:" TK_STRING
		{ $$ = new generic_node ("project_member", $2); delete $1; }
	| "config_header:" filename TK_WHITESPACE
		{ $$ = new generic_node ("project_member", $2); delete $1; delete $3; }
	| section
	;

section
	: "section" string.opt "{" section_members "}"
		{ $$ = new generic_node ("section", $2, $4); delete $1; delete $3; delete $5; }
	;

section_members
	: section_member
		{ $$ = new generic_node ("section_members", $1); }
	| section_members section_member
		{ ($$ = $1)->add ($2); }
	;

section_member
	: ac_checks
		{ $$ = new generic_node ("ac_check", $1); }
	| arg_enable
	| arg_with
	| check_alignof
	| check_cflags
	| check_functions
	| check_headers
	| check_library
	| check_sizeof
	| "error" TK_STRING
		{ $$ = new generic_node ("error", $2); delete $1; }
	| define
	| TK_STRING
		{ $$ = new generic_node ("description", $1); }
	;

ac_checks
	: "c_bigendian"
	| "c_typeof"
	| "c_charset"
	| "c_enum_fwdecl"
	| "c_stmt_exprs"
	| "c_late_expansion"
	| "c_token_paste"
	| "c_float_format"
	| "c_stdint_h"
	;

arg_enable
	: "arg_enable" identifier "=" arg_default "{"
	    section_members
	  "}"
		{ $$ = new generic_node ("arg_enable", $2, $4, $6); delete $1; delete $3; delete $5; delete $7; }
	;

arg_default
	: identifier
	| TK_STRING
	;

arg_with
	: "arg_with" identifier "=" identifier "{"
	    TK_STRING
	    arg_with_options
	  "}"
		{ $$ = new generic_node ("arg_enable", $2, $4, $6, $7); delete $1; delete $3; delete $5; delete $8; }
	;

arg_with_options
	: "options" "{" arg_with_choices "}"
		{ $$ = new generic_node ("arg_enable", $3); delete $1; delete $2; delete $4; }
	;

arg_with_choices
	: arg_with_choice
		{ $$ = new generic_node ("arg_enable", $1); }
	| arg_with_choices arg_with_choice
		{ ($$ = $1)->add ($2); }
	;

arg_with_choice
	: identifier "=>" "{" section_members "}"
		{ $$ = new generic_node ("arg_enable", $1, $4); delete $2; delete $3; delete $5; }
	;

check_alignof
	: "alignof" "{" TK_STRING "}"
		{ $$ = new generic_node ("arg_enable", $3); delete $1; delete $2; delete $4; }
	;

check_cflags
	: "cflags" identifier.opt flags_begin flags flags_end
		{ $$ = new generic_node ("arg_enable", $2, $4); delete $1; }
	;

check_functions
	: "functions" "{" identifiers "}"
		{ $$ = new generic_node ("arg_enable", $3); delete $1; delete $2; delete $4; }
	;

check_headers
	: "headers" "{" filenames.1 TK_WHITESPACE "}"
		{ $$ = new generic_node ("arg_enable", $3); delete $1; delete $2; delete $4; delete $5; }
	;

check_library
	: "library" identifier "{"
	    "symbol:" identifier
	    "header:" filename TK_WHITESPACE
	    check_library_notfound.opt
	    string.opt
	  "}"
		{ $$ = new generic_node ("arg_enable", $2, $5, $7, $9, $10); delete $1; delete $3; delete $4; delete $6; delete $8; delete $11; }
	;

check_library_notfound.opt
	: { $$ = NULL; }
	| "notfound:" TK_STRING
		{ $$ = new generic_node ("arg_enable", $2); delete $1; }
	;

check_sizeof
	: "sizeof" "{" TK_STRING "}"
		{ $$ = new generic_node ("arg_enable", $3); delete $1; delete $2; delete $4; }
	;

define
	: "define" identifier "{" TK_STRING "}"
		{ $$ = new generic_node ("arg_enable", $2, $4); delete $1; delete $3; delete $5; }
	;


/****************************************************************************
 *
 *	Targets (program/library/template)
 *
 ****************************************************************************/
program
	: "program" target_definition
		{ $$ = new generic_node ("program", $2); delete $1; }
	;

library
	: "library" target_definition
		{ $$ = new generic_node ("library", $2); delete $1; }
	;

template
	: "template" target_definition
		{ $$ = new generic_node ("template", $2); delete $1; }
	;

target_definition
	: identifier if.opt inheritance.opt destination.opt target_body
		{ $$ = new generic_node ("target_definition", $1, $2, $3, $4, $5); }
	;

inheritance.opt
	: { $$ = NULL; }
	| inheritance
	;

inheritance
	: ":" identifier
		{ $$ = new generic_node ("inheritance", $2); delete $1; }
	;

destination.opt
	: { $$ = NULL; }
	| destination
	;

destination
	: "->" identifier
		{ $$ = new generic_node ("destination", $2); delete $1; }
	;

target_body
	: "{" target_members "}"
		{ $$ = new generic_node ("target_body", $2); delete $1; delete $3; }
	;

target_members
	: target_member
		{ $$ = new generic_node ("target_members", $1); }
	| target_members target_member
		{ ($$ = $1)->add ($2); }
	;

target_member
	: sources
	| description
	| nodist_sources
	| extra_dist
	| rule
	| link
	| vardecl
	| tool_flags
	;

description
	: TK_STRING
		{ $$ = new generic_node ("description", $1); }
	;

/****************************************************************************
 *
 *	Sources
 *
 ****************************************************************************/
sources
	: "sources" if.opt "{" sources_members "}"
		{ $$ = new generic_node ("sources", $2, $4); delete $1; delete $3; delete $5; }
	;

nodist_sources
	: "nodist_sources" if.opt "{" sources_members "}"
		{ $$ = new generic_node ("nodist_sources", $2, $4); delete $1; delete $3; delete $5; }
	;

extra_dist
	: "extra_dist" if.opt "{" sources_members "}"
		{ $$ = new generic_node ("extra_dist", $2, $4); delete $1; delete $3; delete $5; }
	;

sources_members
	: sources_member
		{ $$ = new generic_node ("sources_members", $1); }
	| sources_members sources_member
		{ ($$ = $1)->add ($2); }
	;

sources_member
	: filename TK_WHITESPACE
		{ $$ = $1; delete $2; }
	| "sources" "(" identifier ")"
		{ $$ = new generic_node ("sources", $3); delete $1; delete $2; delete $4; }
	| "exclude" "{" sources_members "}"
		{ $$ = new generic_node ("exclude", $3); delete $1; delete $2; delete $4; }
	;

/****************************************************************************
 *
 *	Tool flags
 *
 ****************************************************************************/
tool_flags
	: flag_keyword if.opt flags_begin flags flags_end
		{ $$ = new generic_node ("tool_flags", $1, $2, $4); }
	;

flag_keyword
	: identifier
	| "cppflags"
	;

flags_begin
	: "{" { self->lex.push_state (yy::FLAGS); delete $1; }
	;

flags_end
	: "}" { self->lex.pop_state (); delete $1; }
	;

flags
	:
		{ $$ = new generic_node ("flags"); }
	| flags TK_FLAG
		{ ($$ = $1)->add ($2); }
	;

/****************************************************************************
 *
 *	Custom rules
 *
 ****************************************************************************/
rule
	: filenames.1 ":" filenames.0 rule_begin rule_lines rule_end
		{ $$ = new generic_node ("rule", $1, $3, $5); delete $2; }
	;

rule_begin
	: "{" { self->lex.push_state (yy::RULE_CODE); delete $1; }
	;

rule_end
	: "}" { self->lex.pop_state (); delete $1; }
	;

rule_lines
	: rule_line TK_WHITESPACE
		{ $$ = new generic_node ("rule_lines", $1); delete $2; }
	| rule_lines rule_line TK_WHITESPACE
		{ ($$ = $1)->add ($2); delete $3; }
	;

rule_line
	: code_frag
		{ $$ = new generic_node ("rule_line", $1); }
	| rule_line code_frag
		{ ($$ = $1)->add ($2); }
	;

filenames.0
	:
		{ $$ = new generic_node ("filenames"); }
	| filenames.0 filename TK_WHITESPACE
		{ ($$ = $1)->add ($2); delete $3; }
	;

filenames.1
	: filename
		{ $$ = new generic_node ("filenames", $1); }
	| filenames.1 TK_WHITESPACE filename
		{ ($$ = $1)->add ($3); delete $2; }
	;

/****************************************************************************
 *
 *	Link
 *
 ****************************************************************************/
link
	: "link" if.opt link_begin link_body link_end
		{ $$ = new generic_node ("link", $2, $4); delete $1; }
	;

link_begin
	: "{" { self->lex.push_state (yy::LINK); delete $1; }
	;

link_end
	: "}" { self->lex.pop_state (); delete $1; }
	;

link_body
	: link_item
		{ $$ = new generic_node ("link_body", $1); }
	| link_body link_item
		{ ($$ = $1)->add ($2); }
	;

link_item
	: TK_INT_LIB
	| TK_EXT_LIB
	;

/****************************************************************************
 *
 *	Variable declarations
 *
 ****************************************************************************/
vardecl
	: identifier vardecl_begin vardecl_body TK_WHITESPACE
		{ $$ = new generic_node ("vardecl", $1, $3); self->lex.pop_state (); delete $4; }
	;

vardecl_begin
	: "="
		{ self->lex.push_state (yy::VARDECL); delete $1; }
	;

vardecl_body
	: code_frag
		{ $$ = new generic_node ("vardecl_body", $1); }
	| vardecl_body code_frag
		{ ($$ = $1)->add ($2); }
	;

/****************************************************************************
 *
 *	Conditional building
 *
 ****************************************************************************/
if.opt
	: { $$ = NULL; }
	| if
	;

if
	: "if" identifier
		{ $$ = new generic_node ("if", $2); delete $1; }
	;

/****************************************************************************
 *
 *	Tokens
 *
 ****************************************************************************/
string.opt
	: { $$ = NULL; }
	| TK_STRING
	;

identifier.opt
	: { $$ = NULL; }
	| identifier
	;

identifier
	: TK_IDENTIFIER
	;

identifiers
	: identifier
		{ $$ = new generic_node ("identifiers", $1); }
	| identifiers identifier
		{ ($$ = $1)->add ($2); }
	;

filename
	: filename_part
		{ $$ = new generic_node ("filename", $1); }
	| filename filename_part
		{ ($$ = $1)->add ($2); }
	;

filename_part
	: TK_FILENAME
	| TK_IDENTIFIER
	;

code_frag
	: TK_CODE
	| TK_VAR
	;

%%

char const *
tokname (yySymbol yytoken)
{
  return yytname[yytoken];
}
