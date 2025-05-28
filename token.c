#include <stdio.h>

#include "token.h"

void TokenFini(Token *t) {
    if (t->type == TOKEN_STRING)
       StrFini(&t->literal.str);
}

static void print_str(StrView str) {
    for (size_t i = 0; i < str.len; i++)
        putchar(str.str[i]);
}

void TokenPrint(const Token *t) {
    if (t->type == TOKEN_EOF) {
        printf("EOF\n");
        return;
    }

    printf("%d, '",  t->type);
    print_str(t->lexeme);
    printf("'");

    switch (t->type) {
    case TOKEN_STRING:
        printf(", '%s', len: %zu", t->literal.str.str, t->literal.str.len);
        break;

    case TOKEN_NUMBER:
        printf(", %lf", t->literal.f);
        break;

    default:
        break;
    }

    putchar('\n');
}
