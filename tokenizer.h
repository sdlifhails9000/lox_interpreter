#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <stddef.h>

#include "token.h"

#define MAX_TOKENS 1024

typedef struct {
    Token array[MAX_TOKENS];
    int len;
} TokenArray;

typedef struct {
    char *source;
    size_t source_len;
    size_t start;
    size_t current;
    size_t line;

    TokenArray tokens;
} Tokenizer;

Tokenizer TokenizerInit(const char *source, size_t len);
void TokenizerFini(Tokenizer *t);

const TokenArray *TokenizerGetAllTokens(Tokenizer *t);
void TokenizerGetToken(Tokenizer *t);

#endif
