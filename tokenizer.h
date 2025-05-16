#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <stddef.h>

#include "token.h"

#define MAX_TOKENS 1024

typedef struct {
    char *source;
    size_t source_len;
    size_t start;
    size_t current;
    size_t line;

    Token tokens[MAX_TOKENS];
    size_t n_tokens;
} Tokenizer;

Tokenizer TokenizerInit(const char *source, size_t len);
void TokenizerFini(Tokenizer *t);

const Token *TokenizerGetAllTokens(Tokenizer *t, size_t *len);
void TokenizerGetToken(Tokenizer *t);

#endif
