#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <stddef.h>

#include "token.h"
#include "map.h"
#include "str.h"

#define MAX_TOKENS 1024

typedef struct {
    Token array[MAX_TOKENS];
    size_t len;
} TokenArray;

typedef struct {
    Str source;
    size_t start;
    size_t current;
    size_t line;

    TokenArray tokens;
    Map reserved;
} Tokenizer;

Tokenizer TokenizerInit(Str source);
void TokenizerFini(Tokenizer *t);

const TokenArray *TokenizerGetAllTokens(Tokenizer *t);
void TokenizerGetToken(Tokenizer *t);

#endif
