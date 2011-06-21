#pragma once

#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <inttypes.h>

#include "node.h"
#include "yyparse.h"
#ifndef FLEX_SCANNER
#include "yylex.h"
#endif

struct parser;
typedef short yySymbol;

void yyerror (YYLTYPE const *llocp, parser *parse, char const *msg);
char const *tokname (yySymbol yytoken);
