#include <stdio.h>

#include "token.h"

static inline void print_str(const char *str, size_t len) {
    for (size_t i = 0; i < len; i++)
        putchar(str[i]);
}

void TokenFini(Token *t) {
    if (t->type == TOKEN_STRING)
       free(t->literal.str);
}

void TokenPrint(const Token *t) {
    if (t->type == TOKEN_EOF) {
        printf("EOF\n");
        return;
    }

    printf("%d, '",  t->type);
    print_str(t->lexeme, t->lexeme_len);
    printf("'");

    switch (t->type) {
    case TOKEN_STRING:
        printf(", '%s', len: %zu", t->literal.str, t->lexeme_len - 2);
        break;

    case TOKEN_NUMBER:
        printf(", %lf", t->literal.f);
        break;

    default:
        break;
    }

    putchar('\n');
}
