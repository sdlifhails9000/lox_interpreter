#include "token.h"
#include "expr.h"

typedef struct {
   TokenArray *tokens;
   size_t current;
} Parser;

Parser ParserInit(TokenArray *tokens);
Expr *ParserParse(Parser *p);

