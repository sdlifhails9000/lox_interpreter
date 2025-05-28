#ifndef TOKEN_H_
#define TOKEN_H_

#include <stdio.h>
#include <stdlib.h>

#include "str.h"

#define MAX_TOKENS 4096

typedef enum {
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN, TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE, TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,

    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE, TOKEN_FUN, TOKEN_FOR,
    TOKEN_IF, TOKEN_NIL, TOKEN_OR, TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER,
    TOKEN_THIS, TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

    TOKEN_EOF,
} TokenType;

typedef struct {
    TokenType type;
    StrView lexeme;
    union {
        Str str;
        double f;
    } literal;
    int line;
} Token;

typedef struct {
    Token array[MAX_TOKENS];
    size_t len;
} TokenArray;

void TokenFini(Token *t);

void TokenPrint(const Token *t);

#endif

