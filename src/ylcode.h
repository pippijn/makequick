#pragma once

#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <inttypes.h>

#include "node.h"

typedef location YYLTYPE;
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1

struct parser;

#include "yyparse.hpp"
typedef struct lexer *YY_EXTRA_TYPE;
#define YY_EXTRA_TYPE YY_EXTRA_TYPE
#ifndef FLEX_SCANNER
#include "yylex.hpp"
#endif

typedef short yySymbol;

void yyerror (YYLTYPE const *llocp, parser *parse, char const *msg);
char const *tokname (yySymbol yytoken);
