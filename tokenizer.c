#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "tokenizer.h"
#include "token.h"
#include "logging.h"

/* ---- HELPER FUNCTIONS ---- */

static inline bool isWhitespace(char c) {
    return c == ' ' || c == '\r' || c == '\t' || c == '\n';
}

static inline bool isDigit(char c) {
    return c <= '9' && c >= '0';
}

static inline bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'z') ||
           (c == '_');
}

static inline bool isAlphaNum(char c) {
    return isAlpha(c) || isDigit(c);
}

static bool isAtEnd(const Tokenizer *t) {
    return t->current >= t->source_len;
}

static char peek(const Tokenizer *t) {
    if (isAtEnd(t))
        return '\0';
    
    return t->source[t->current];
}

static char peekNext(const Tokenizer *t) {
    if (t->current + 1 >= t->source_len)
        return '\0';
    return t->source[t->current + 1];
}

static char readChar(Tokenizer *t) {
    char c = t->source[t->current];
    t->current++;

    return c;
}

static void addToken(Tokenizer *t, TokenType type) {
    Token token = {
        .type = type,
        .line = t->line
    };

    token.lexeme.p = &t->source[t->start];
    token.lexeme.str_len = t->current - t->start;

    switch (type) {
    case TOKEN_STRING:
        token.literal.str.p = &t->source[t->start + 1];
        token.literal.str.str_len = t->current - t->start - 2;
        break;

    case TOKEN_NUMBER:
        token.literal.f = atof(&t->source[t->start]);
        break;

    default:
        break;
    }

    t->tokens.array[t->tokens.len] = token;
    t->tokens.len++;
}

static void string(Tokenizer *t) {
    while (peek(t) != '"' && !isAtEnd(t)) {
        if (peek(t) == '\n')
            t->line++;
        readChar(t);
    }

    if (isAtEnd(t)) {
        error(t->line, "Unterminated string.\n");
        return;
    }

    readChar(t);

    addToken(t, TOKEN_STRING);
}

static void number(Tokenizer *t) {
    while (isDigit(peek(t)))
        readChar(t);

    if (peek(t) == '.' && isDigit(peekNext(t))) {
        readChar(t);
        while (isDigit(peek(t)))
            readChar(t);
    }

    addToken(t, TOKEN_NUMBER);
}

static void identifier(Tokenizer *t) {
    char *ident;
    TokenType type;

    while (isAlphaNum(peek(t)))
        readChar(t);

    ident = malloc(t->current - t->start);
    for (size_t i = 0; i < t->current - t->start; i++)
        ident[i] = t->source[i + t->start];

    if ((type = MapGet(&t->reserved, ident)) == ~(unsigned int)0)
        type = TOKEN_IDENTIFIER;

    free(ident);
    addToken(t, type);
}

static bool match(Tokenizer *t, char expected) {
    if (isAtEnd(t))
        return false;

    if (expected != t->source[t->current])
        return false;

    t->current++;
    return true;
}

static void skipMultiline(Tokenizer *t) {
    while (!isAtEnd(t)) {
        char c = readChar(t);
        if (c != '*')
            continue;

        if (peek(t) == '/') {
            readChar(t);
            return;
        }
    }    

    if (isAtEnd(t))
        error(t->line, "Unmatched multiline comment.\n");
}

static inline void skipSingleline(Tokenizer *t) {
    while (peek(t) != '\n' && !isAtEnd(t))
        readChar(t);
}

static void skipWhitespace(Tokenizer *t) {
    while (isWhitespace(peek(t)) && !isAtEnd(t))
        readChar(t);
}

/* ---- MAIN DEFINITIONS ---- */

Tokenizer TokenizerInit(const char *source, size_t len) {
    Tokenizer t = {
        .source = strdup(source),
        .source_len = len,
        .start = 0,
        .current = 0,
        .line = 1,
        .tokens.len = 0,
        .reserved = MapInit()
    };

    for (int i = 0; i < MAX_TOKENS; i++)
        memset(&t.tokens.array[i], 0x00, sizeof(Token));

    MapSet(&t.reserved, "and",    TOKEN_AND);
    MapSet(&t.reserved, "class",  TOKEN_CLASS);
    MapSet(&t.reserved, "else",   TOKEN_ELSE);
    MapSet(&t.reserved, "false",  TOKEN_FALSE);
    MapSet(&t.reserved, "for",    TOKEN_FOR);
    MapSet(&t.reserved, "fun",    TOKEN_FUN);
    MapSet(&t.reserved, "if",     TOKEN_IF);
    MapSet(&t.reserved, "nil",    TOKEN_NIL);
    MapSet(&t.reserved, "or",     TOKEN_OR);
    MapSet(&t.reserved, "print",  TOKEN_PRINT);
    MapSet(&t.reserved, "return", TOKEN_RETURN);
    MapSet(&t.reserved, "super",  TOKEN_SUPER);
    MapSet(&t.reserved, "this",   TOKEN_THIS);
    MapSet(&t.reserved, "true",   TOKEN_TRUE);
    MapSet(&t.reserved, "let",    TOKEN_LET);
    MapSet(&t.reserved, "while",  TOKEN_WHILE);

    return t;
}

void TokenizerFini(Tokenizer *t) {
    MapFini(&t->reserved);
    free(t->source);
}

void TokenizerGetToken(Tokenizer *t) {
    char c = readChar(t);

    switch (c) {
    case '(': addToken(t, TOKEN_LEFT_PAREN); break;
    case ')': addToken(t, TOKEN_RIGHT_PAREN); break;
    case '{': addToken(t, TOKEN_LEFT_BRACE); break;
    case '}': addToken(t, TOKEN_RIGHT_BRACE); break;
    case ',': addToken(t, TOKEN_COMMA); break;
    case '.': addToken(t, TOKEN_DOT); break;
    case '-': addToken(t, TOKEN_MINUS); break;
    case '+': addToken(t, TOKEN_PLUS); break;
    case ';': addToken(t, TOKEN_SEMICOLON); break;
    case '*': addToken(t, TOKEN_STAR); break;
    case '"': string(t); break;
    case '=':
        addToken(t, match(t, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        break;

    case '!':
        addToken(t, match(t, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        break;

    case '<':
        addToken(t, match(t, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        break;

    case '>':
        addToken(t, match(t, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        break;

    case '/':
        if (match(t, '/')) {
            skipSingleline(t);
        } else if (match(t, '*')) {
            skipMultiline(t);
        } else {
            addToken(t, TOKEN_SLASH);
            break;
        }

        t->start = t->current;
        if (isAtEnd(t))
            break;
        
        TokenizerGetToken(t);
        return;

    case '\n':
        t->line++;
        /* fallthrough */

    case '\r':
    case '\t':
    case ' ':
        skipWhitespace(t);
        t->start = t->current;
        if (isAtEnd(t))
            break;

        TokenizerGetToken(t);
        return;

    default:
        if (isDigit(c))
            number(t);
        else if (isAlpha(c))
            identifier(t);
        else
            error(t->line, "Unexpected character.\n");
        break;
    }
}

const TokenArray *TokenizerGetAllTokens(Tokenizer *t) {
    while (!isAtEnd(t)) {
        t->start = t->current;
        TokenizerGetToken(t);
    }

    addToken(t, TOKEN_EOF);

    return &t->tokens;
}

