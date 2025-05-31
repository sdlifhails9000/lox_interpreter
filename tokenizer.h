#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <stddef.h>

#include "token.h"
#include "map.h"

typedef struct {
    char *source;
    size_t source_len;
    size_t start;
    size_t current;
    size_t line;

    TokenArray tokens;
    Map reserved;
} Tokenizer;

Tokenizer TokenizerInit(char *source, size_t len);
void TokenizerFini(Tokenizer *t);

const TokenArray *TokenizerGetAllTokens(Tokenizer *t);
void TokenizerGetToken(Tokenizer *t);

#endif
