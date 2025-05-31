#include "token.h"
#include "expr.h"

typedef struct {
   const TokenArray *tokens;
   size_t current;
} Parser;

Parser ParserInit(const TokenArray *tokens);
Expr *ParserParse(Parser *p);

