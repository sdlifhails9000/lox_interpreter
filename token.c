#include <stdio.h>
#include <string.h>

#include "token.h"

Token TokenInit(TokenType type, char *lexeme, void *literal, int line) {
    Token token = {
        .type = type,
        .line = line
    };

    switch (type) {
    case TOKEN_EOF:
        return token;

    case TOKEN_STRING:
        token.literal.str.p = (char*)strdup(literal);
        token.literal.str.str_len = strlen(token.literal.str.p);
        break;

    case TOKEN_NUMBER:
        token.literal.f = *(double*)literal;
        break;

    default:
        break;
    }

    if (lexeme != NULL)
        token.lexeme = strdup(lexeme);

    return token;
}

void TokenFini(Token *t) {
    free(t->lexeme);
    if (t->type == TOKEN_STRING)
        free(t->literal.str.p);
}

void TokenPrint(const Token *t) {
    if (t->type == TOKEN_EOF) {
        printf("EOF\n");
        return;
    }

    printf("%d, '%s'", t->type, t->lexeme);

    switch (t->type) {
    case TOKEN_STRING:
        printf(", \'");
        for (size_t i = 0; i < t->literal.str.str_len; i++)
            putchar(t->literal.str.p[i]);
        printf("\', len: %d", t->literal.str.str_len);
        break;

    case TOKEN_NUMBER:
        printf(", %lf", t->literal.f);
        break;

    default:
        break;
    }

    putchar('\n');
}
