#include "lexer.h"
#include "expr.h"

typedef struct {
   const Token *tokens;
   size_t current;
} Parser;

Parser ParserInit(const Token tokens[]);
Expr *ParserParse(Parser *p);
