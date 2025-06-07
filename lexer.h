#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <stddef.h>

#include "object.h"

#define TABLE_SIZE 100

typedef enum {
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN, TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE, TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_COLON, TOKEN_SLASH, TOKEN_STAR, TOKEN_QUESTION,

    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,

    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE, TOKEN_FUN, TOKEN_FOR,
    TOKEN_IF, TOKEN_NIL, TOKEN_OR, TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER,
    TOKEN_THIS, TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

    TOKEN_EOF,

    TOKEN_ILLEGAL
} TokenType;

typedef struct {
    TokenType type;
    const char *lexeme;
    size_t lexeme_len;
    int line;
} Token;

Token TokenInit(TokenType type, const char *lexeme, size_t lexeme_len);

#define TokenIllegal (Token){ .type = TOKEN_ILLEGAL }
#define TokenEOF (Token){ .type = TOKEN_EOF }

typedef struct {
    char *key;
    TokenType value;
    void *next;
} __Entry;

typedef struct {
    __Entry *buckets[TABLE_SIZE];
} __Map;

typedef struct {
    const char *source;
    size_t source_len;
    size_t start;
    size_t current;
    size_t line;
    __Map reserved;
} Lexer;

Lexer LexerInit(const char *source, size_t len);
void LexerFini(Lexer *l);
Token LexerGetToken(Lexer *l);
bool LexerIsDone(const Lexer* l);

#endif

