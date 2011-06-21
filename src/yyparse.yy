%{
#include "parser.h"
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
%glr-parser

%parse-param { parser *self }
%lex-param { parser *self }

%token TK_EOF 0			"end of file"

%token KW_NODIST_SOURCES	"nodist_sources"
%token KW_SOURCES		"sources"

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
	: definition_list
	;

definition_list
	: /* empty */
	| definition_list definition
	;

definition
	: TK_IDENTIFIER
	| TK_INTEGER
	| TK_KEYWORD
	| TK_OPERATOR
	| TK_STRING
	;

toplevel_declarations
	: toplevel_declaration
	| toplevel_declarations toplevel_declaration
	;


/****************************************************************************
 *
 *	Project definition with configure checks and arguments
 *
 ****************************************************************************/
toplevel_declaration: project_block;

project_block
	: "project" string "{" project_members "}"
	;

project_members
	: "version:" string
	  "contact:" string
	  "config_header:" filename
	  project_sections
	;

project_sections
	: project_section
	| project_sections project_section
	;

project_section
	: "section" string "{" project_section_members "}"
	| "section"        "{" project_section_members "}"
	;

project_section_members
	: project_section_member
	| project_section_members project_section_member
	;

project_section_member
	: project_library
	| project_headers
	| project_functions
	| project_arg_enable
	| project_arg_with
	| project_cflags
	| project_check
	;

project_library
	: "library" identifier "{" project_library_members "}"
	;

project_library_members
	: "symbol:" identifier
	  "header:" filename
	;

project_headers
	: "headers" "{" filenames "}"
	;

project_functions
	: "functions" "{" identifiers "}"
	;

project_arg_enable
	: "arg_enable" identifier "=" identifier "{" project_arg_enable_members "}"
	| "arg_enable" identifier "=" string     "{" project_arg_enable_members "}"
	;

project_arg_enable_members
	: string
	  project_section_members
	;

project_arg_with
	: "arg_with" identifier "=" identifier "{" project_arg_with_members "}"
	;

project_arg_with_members
	: string
	  project_arg_with_options
	;

project_arg_with_options
	: "options" "{" project_arg_with_options_members "}"
	;

project_arg_with_options_members
	: project_arg_with_options_member
	| project_arg_with_options_members project_arg_with_options_member
	;

project_arg_with_options_member
	: identifier "=>" "{" project_section_members "}"
	;

project_cflags
	: cflags
	| "cflags" identifier cflags_body
	;

project_check
	: "c_bigendian"
	| "c_typeof"
	| "c_charset"
	| "c_enum_fwdecl"
	| "c_stmt_exprs"
	| "c_late_expansion"
	| "c_token_paste"
	| "c_float_format"
	| "c_stdint_h"
	| "alignof" "{" strings "}"
	| "sizeof" "{" strings "}"
	;


cflags
	: "cflags" cflags_body
	;

cflags_body
	: "{" identifiers "}"
	;


/****************************************************************************
 *
 *	Targets (program/library/template)
 *
 ****************************************************************************/
toplevel_declaration: program | library | template;

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
	: identifier condition inheritance target_body
	;

inheritance
	: ":" identifiers_comma
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
	| link
	| variable_definition
	| rule
	;

sources
	: "sources" condition "{" sources_members "}"
	;

sources_members
	: sources_member
	| sources_members sources_member
	;

sources_member
	: sources_ref
	| filename
	;

sources_ref
	: "sources" "(" identifier ")"
	;

link
	: "link" condition "{" library_refs "}"
	;

condition
	: "if" identifier
	;

variable_definition
	: identifier "=" string
	;

rule
	: patterns ":" patterns "{" rule_members "}"
	;

rule_members
	: rule_member
	| rule_members rule_member
	;

rule_member
	: string
	;

pattern
	: identifier
	;

/****************************************************************************
 *
 *	Common rules
 *
 ****************************************************************************/
patterns
	: pattern
	| patterns pattern
	;

library_refs
	: library_ref
	| library_refs library_ref
	;

filenames
	: filename
	| filenames filename
	;

identifiers
	: identifier
	| identifiers identifier
	;

identifiers_comma
	: identifier
	| identifiers_comma "," identifier
	;

strings
	: string
	| strings string
	;


/****************************************************************************
 *
 *	Tokens
 *
 ****************************************************************************/
string: TK_STRING
identifier: TK_IDENTIFIER
library_ref: TK_LIBRARY_REF
filename: TK_FILENAME

%%

char const *
yytokname (yySymbol yytoken)
{
  return yytokenName (yytoken);
}
