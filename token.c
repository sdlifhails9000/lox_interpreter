#include <stdio.h>

#include "token.h"

static void print_str(const StrView str) {
    for (size_t i = 0; i < str.str_len; i++)
        putchar(str.p[i]);
}

void TokenPrint(const Token *t) {
    if (t->type == TOKEN_EOF) {
        printf("EOF\n");
        return;
    }

    printf("%d, '",  t->type);
    print_str(t->lexeme);
    putchar('\'');

    switch (t->type) {
    case TOKEN_STRING:
        printf(", '");
        for (size_t i = 0; i < t->literal.str.str_len; i++)
            putchar(t->literal.str.p[i]);
        printf("', len: %zu", t->literal.str.str_len);
        break;

    case TOKEN_NUMBER:
        printf(", %lf", t->literal.f);
        break;

    default:
        break;
    }

    putchar('\n');
}
