%{
#include "parser.h"
#define YYSTYPE YYSTYPE
%}

%union {
  nodes::node *node;
  nodes::node_list *list;
  tokens::token *token;
}

%{
#include "yystate.h"

union YYSTYPE;
struct YYLTYPE;
static int
yylex (YYSTYPE *yylval, YYLTYPE *yylloc, parser *self)
{
  return self->lex.next (yylval, yylloc);
}

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
 * If N is 0, then set CURRENT to the empty location which ends
 * the previous symbol: RHS[0] (always defined).  */
#define YYLLOC_DEFAULT(Current, Rhs, N)					\
    do {								\
      (Current) = YYRHSLOC (Rhs, 1);					\
      if (YYID (N))							\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    } while (YYID (0))

/* YY_LOCATION_PRINT -- Print the location on the stream. */
#define YY_LOCATION_PRINT(File, Loc)			\
    fprintf (File, "%d.%d-%d.%d",			\
	     (Loc).first_line, (Loc).first_column,	\
	     (Loc).last_line,  (Loc).last_column)

using namespace nodes;
%}

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
%token<token> TK_DOT			"."
%token<token> TK_DOLLAR			"$"
%token<token> TK_COLON			":"
%token<token> TK_SEMICOLON		";"
%token<token> TK_EQUALS			"="
%token<token> TK_ARROW			"->"
%token<token> TK_DARROW			"=>"
%token<token> TK_WHITESPACE		"whitespace"

%token<token> TK_FN_QMARK		"?"
%token<token> TK_FN_SLASH		"/"
%token<token> TK_FN_DOT			"'.'"
%token<token> TK_FN_LBRACE		"'{'"
%token<token> TK_FN_RBRACE		"'}'"
%token<token> TK_FN_STAR		"*"
%token<token> TK_FN_STARSTAR		"**"
%token<token> TK_FN_PERCENT		"%"
%token<token> TK_FN_PERPERCENT		"%%"

%token<token> TK_EXT_LIB		"external library"
%token<token> TK_INT_LIB		"internal library"

%token<token> TK_CODE			"shell code"
%token<token> TK_FILENAME		"filename"
%token<token> TK_FLAG			"tool flag"
%token<token> TK_IDENTIFIER		"identifier"
%token<token> TK_INTEGER		"integer literal"
%token<token> TK_STRING			"string literal"
%token<token> TK_SHORTVAR		"short variable name"

%token<token> TK_ERROR			"invalid character"

%token R_FILENAME

%type<list> sources target_definition
%type<list> program library template target_members toplevel_declarations toplevel_declaration
%type<list> filename filenames.0 filenames.1 sources_member sources_members extra_dist nodist_sources
%type<list> rules rule rule_lines rule_line link link_body vardecl vardecl_body
%type<list> inheritance.opt inheritance if.opt if destination.opt destination
%type<list> tool_flags flags identifiers
%type<list> check_alignof check_cflags check_functions check_headers check_library check_sizeof
%type<list> define arg_enable arg_enable_options arg_enable_choices
%type<list> arg_enable_choice arg_enable_content project project_members project_member
%type<list> variable variable_content section section_members
%type<node> filename_part section_member target_member code_frag
%type<token> identifier filename_token link_item string.opt identifier.opt ac_checks
%type<token> arg_default check_library_notfound.opt

%destructor { delete $$; } <*>

%%
/****************************************************************************
 *
 *	Toplevel declarations
 *
 ****************************************************************************/
document
	: toplevel_declarations
		{ self->doc = make_node<n_document> (@$, $1); }
	| R_FILENAME filename
		{ self->doc = $2; }
	;

toplevel_declarations
	: toplevel_declaration
		{ $$ = make_node<n_toplevel_declarations> (@$, $1); }
	| toplevel_declarations toplevel_declaration
		{ ($$ = $1)->add ($2)->loc = @$; }
	;

toplevel_declaration
	: project
	| program
	| library
	| template
	| extra_dist
	| "global" "{" rules "}"
		{ $$ = $3; delete $1; delete $2; delete $4; }
	;


/****************************************************************************
 *
 *	Project configuration
 *
 ****************************************************************************/
project
	: "project" TK_STRING "{" project_members "}"
		{ $$ = make_node<n_project> (@$, $2, $4); delete $1; delete $3; delete $5; }
	;

project_members
	: project_member
		{ $$ = make_node<n_project_members> (@$, $1); }
	| project_members project_member
		{ ($$ = $1)->add ($2)->loc = @$; }
	;

project_member
	: "version:" TK_STRING
		{ $$ = make_node<n_project_version> (@$, $2); delete $1; }
	| "contact:" TK_STRING
		{ $$ = make_node<n_project_contact> (@$, $2); delete $1; }
	| "config_header:" filename TK_WHITESPACE
		{ $$ = make_node<n_project_header> (@$, $2); delete $1; delete $3; }
	| section
	;

section
	: "section" string.opt "{" section_members "}"
		{ $$ = make_node<n_section> (@$, $2, $4); delete $1; delete $3; delete $5; }
	;

section_members
	: section_member
		{ $$ = make_node<n_section_members> (@$, $1); }
	| section_members section_member
		{ ($$ = $1)->add ($2)->loc = @$; }
	;

section_member
	: ac_checks
		{ $$ = make_node<n_ac_check> (@$, $1); }
	| arg_enable
		{ $$ = $1; }
	| check_alignof
		{ $$ = $1; }
	| check_cflags
		{ $$ = $1; }
	| check_functions
		{ $$ = $1; }
	| check_headers
		{ $$ = $1; }
	| check_library
		{ $$ = $1; }
	| check_sizeof
		{ $$ = $1; }
	| "error" TK_STRING
		{ $$ = make_node<n_error> (@$, $2); delete $1; }
	| define
		{ $$ = $1; }
	| TK_STRING
		{ $$ = $1; }
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
	    TK_STRING
	    arg_enable_content
	  "}"
		{ $$ = make_node<n_arg_enable> (@$, $2, $4, $6, $7); delete $1; delete $3; delete $5; delete $8; }
	;

arg_default
	: identifier
	| TK_STRING
	;

arg_enable_content
	: { $$ = NULL; }
	| section_members
	| arg_enable_options
	;

arg_enable_options
	: "options" "{" arg_enable_choices "}"
		{ $$ = $3; delete $1; delete $2; delete $4; }
	;

arg_enable_choices
	: arg_enable_choice
		{ $$ = make_node<n_arg_enable_choices> (@$, $1); }
	| arg_enable_choices arg_enable_choice
		{ ($$ = $1)->add ($2)->loc = @$; }
	;

arg_enable_choice
	: identifier "=>" "{" section_members "}"
		{ $$ = make_node<n_arg_enable_choice> (@$, $1, $4); delete $2; delete $3; delete $5; }
	;

check_alignof
	: "alignof" "{" TK_STRING "}"
		{ $$ = make_node<n_check_alignof> (@$, $3); delete $1; delete $2; delete $4; }
	;

check_cflags
	: "cflags" identifier.opt flags_begin flags flags_end
		{ $$ = make_node<n_check_cflags> (@$, $2, $4); delete $1; }
	;

check_functions
	: "functions" "{" identifiers "}"
		{ $$ = make_node<n_check_functions> (@$, $3); delete $1; delete $2; delete $4; }
	;

check_headers
	: "headers" "{" filenames.1 TK_WHITESPACE "}"
		{ $$ = make_node<n_check_headers> (@$, $3); delete $1; delete $2; delete $4; delete $5; }
	;

check_library
	: "library" identifier "{"
	    "symbol:" identifier
	    "header:" filename TK_WHITESPACE
	    check_library_notfound.opt
	    string.opt
	  "}"
		{ $$ = make_node<n_check_library> (@$, $2, $5, $7, $9, $10); delete $1; delete $3; delete $4; delete $6; delete $8; delete $11; }
	;

check_library_notfound.opt
	: { $$ = NULL; }
	| "notfound:" TK_STRING
		{ $$ = $2; delete $1; }
	;

check_sizeof
	: "sizeof" "{" TK_STRING "}"
		{ $$ = make_node<n_check_sizeof> (@$, $3); delete $1; delete $2; delete $4; }
	;

define
	: "define" identifier "{" TK_STRING "}"
		{ $$ = make_node<n_define> (@$, $2, $4); delete $1; delete $3; delete $5; }
	;


/****************************************************************************
 *
 *	Targets (program/library/template)
 *
 ****************************************************************************/
program
	: "program" target_definition
		{ $$ = make_node<n_program> (@$, $2); delete $1; }
	;

library
	: "library" target_definition
		{ $$ = make_node<n_library> (@$, $2); delete $1; }
	;

template
	: "template" target_definition
		{ $$ = make_node<n_template> (@$, $2); delete $1; }
	;

target_definition
	: identifier if.opt inheritance.opt destination.opt "{" target_members "}"
		{ $$ = make_node<n_target_definition> (@$, $1, $2, $3, $4, $6); delete $5; delete $7; }
	;

inheritance.opt
	: { $$ = NULL; }
	| inheritance
	;

inheritance
	: ":" identifier
		{ $$ = make_node<n_inheritance> (@$, $2); delete $1; }
	;

destination.opt
	: { $$ = NULL; }
	| destination
	;

destination
	: "->" identifier
		{ $$ = make_node<n_destination> (@$, $2); delete $1; }
	;

target_members
	: target_member
		{ $$ = make_node<n_target_members> (@$, $1); }
	| target_members target_member
		{ ($$ = $1)->add ($2)->loc = @$; }
	;

target_member
	: sources		{ $$ = $1; }
	| TK_STRING		{ $$ = $1; }
	| nodist_sources	{ $$ = $1; }
	| extra_dist		{ $$ = $1; }
	| rule			{ $$ = $1; }
	| link			{ $$ = $1; }
	| vardecl		{ $$ = $1; }
	| tool_flags		{ $$ = $1; }
	;

/****************************************************************************
 *
 *	Sources
 *
 ****************************************************************************/
sources
	: "sources" if.opt "{" sources_members "}"
		{ $$ = make_node<n_sources> (@$, $2, $4); delete $1; delete $3; delete $5; }
	;

nodist_sources
	: "nodist_sources" if.opt "{" sources_members "}"
		{ $$ = make_node<n_nodist_sources> (@$, $2, $4); delete $1; delete $3; delete $5; }
	;

extra_dist
	: "extra_dist" if.opt "{" sources_members "}"
		{ $$ = make_node<n_extra_dist> (@$, $2, $4); delete $1; delete $3; delete $5; }
	;

sources_members
	: sources_member
		{ $$ = make_node<n_sources_members> (@$, $1); }
	| sources_members sources_member
		{ ($$ = $1)->add ($2)->loc = @$; }
	;

sources_member
	: filename TK_WHITESPACE
		{ $$ = $1; delete $2; }
	| "sources" "(" identifier ")"
		{ $$ = make_node<n_sourcesref> (@$, $3); delete $1; delete $2; delete $4; }
	| "exclude" "{" sources_members "}"
		{ $$ = make_node<n_exclude> (@$, $3); delete $1; delete $2; delete $4; }
	;

/****************************************************************************
 *
 *	Tool flags
 *
 ****************************************************************************/
tool_flags
	: identifier if.opt flags_begin flags flags_end
		{ $$ = make_node<n_tool_flags> (@$, $1, $2, $4); }
	;

flags_begin
	: "{" { self->lex.push_state (yy::FLAGS); delete $1; }
	;

flags_end
	: "}" { self->lex.pop_state (); delete $1; }
	;

flags
	:
		{ $$ = make_node<n_flags> (@$); }
	| flags TK_FLAG
		{ ($$ = $1)->add ($2)->loc = @$; }
	;

/****************************************************************************
 *
 *	Custom rules
 *
 ****************************************************************************/
rules
	: rule
		{ $$ = make_node<n_rules> (@$, $1); }
	| rules rule
		{ ($$ = $1)->add ($2)->loc = @$; }
	;

rule
	: filenames.1 ":" filenames.0 rule_begin rule_lines rule_end
		{ $$ = make_node<n_rule> (@$, $1, $3, $5); delete $2; }
	| filenames.1 ":" filenames.0 ";"
		{ $$ = make_node<n_rule> (@$, $1, $3,  0); delete $2; delete $4; }
	;

rule_begin
	: "{" { self->lex.push_state (yy::RULE_CODE); delete $1; }
	;

rule_end
	: "}" { self->lex.pop_state (); delete $1; }
	;

rule_lines
	: rule_line TK_WHITESPACE
		{ $$ = make_node<n_rule_lines> (@$, $1); delete $2; }
	| rule_lines rule_line TK_WHITESPACE
		{ ($$ = $1)->add ($2)->loc = @$; delete $3; }
	;

rule_line
	: code_frag
		{ $$ = make_node<n_rule_line> (@$, $1); }
	| rule_line code_frag
		{ ($$ = $1)->add ($2)->loc = @$; }
	;

filenames.0
	:
		{ $$ = make_node<n_filenames> (@$); }
	| filenames.0 filename TK_WHITESPACE
		{ ($$ = $1)->add ($2)->loc = @$; $$->loc.file = $2->loc.file; delete $3; }
	;

filenames.1
	: filename
		{ $$ = make_node<n_filenames> (@$, $1); }
	| filenames.1 TK_WHITESPACE filename
		{ ($$ = $1)->add ($3)->loc = @$; delete $2; }
	;

/****************************************************************************
 *
 *	Link
 *
 ****************************************************************************/
link
	: "link" if.opt link_begin link_body link_end
		{ $$ = make_node<n_link> (@$, $2, $4); delete $1; }
	;

link_begin
	: "{" { self->lex.push_state (yy::LINK); delete $1; }
	;

link_end
	: "}" { self->lex.pop_state (); delete $1; }
	;

link_body
	: link_item
		{ $$ = make_node<n_link_body> (@$, $1); }
	| link_body link_item
		{ ($$ = $1)->add ($2)->loc = @$; }
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
		{ $$ = make_node<n_vardecl> (@$, $1, $3); self->lex.pop_state (); delete $4; }
	;

vardecl_begin
	: "="
		{ self->lex.push_state (yy::VARDECL); delete $1; }
	;

vardecl_body
	: code_frag
		{ $$ = make_node<n_vardecl_body> (@$, $1); }
	| vardecl_body code_frag
		{ ($$ = $1)->add ($2)->loc = @$; }
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
		{ $$ = make_node<n_if> (@$, $2); delete $1; }
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
		{ $$ = make_node<n_identifiers> (@$, $1); }
	| identifiers identifier
		{ ($$ = $1)->add ($2)->loc = @$; }
	;

filename
	: filename_part
		{ $$ = make_node<n_filename> (@$, $1); }
	| filename filename_part
		{ ($$ = $1)->add ($2)->loc = @$; }
	;

filename_part
	: filename_token
		{ $$ = $1; }
	| "$" variable
		{ $$ = $2; delete $1; }
	;

filename_token
	: TK_FILENAME
	| TK_IDENTIFIER
	| "?"
	| "/"
	| "'.'"
	| "'{'"
	| "'}'"
	| "*"
	| "**"
	| "%"
	| "%%"
	;

code_frag
	: TK_CODE
		{ $$ = $1; }
	| "$" variable
		{ $$ = $2; delete $1; }
	;

variable
	: TK_SHORTVAR
		{ $$ = make_node<n_variable> (@$, $1); }
	| TK_INTEGER
		{ $$ = make_node<n_variable> (@$, $1); }
	| "(" variable_content ")"
		{ $$ = make_node<n_variable> (@$, $2); delete $1; delete $3; }
	| "[" identifier "]"
		{ $$ = make_node<n_variable> (@$, $2); delete $1; delete $3; }
	;

variable_content
	: identifier
		{ $$ = make_node<n_variable_content> (@$, $1,  0,  0); }
	| identifier "." identifier
		{ $$ = make_node<n_variable_content> (@$, $1, $3,  0); delete $2; }
	| identifier "." identifier ":" identifier
		{ $$ = make_node<n_variable_content> (@$, $1, $3, $5); delete $2; delete $4; }
	;

%%

char const *
tokname (yySymbol yytoken)
{
  return yytname[yytoken - 255];
}
