#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "tokenizer.h"
#include "token.h"
#include "logging.h"

/*
 * TODO: implement a string and a stringview.
 */

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

static void addToken(Tokenizer *t, TokenType type, void *literal, int str_len) {
    char *lexeme = malloc(t->current - t->start + 1);
    lexeme[t->current - t->start] = '\0';
    strncpy(lexeme, &(t->source[t->start]), t->current - t->start);

    if (type == TOKEN_STRING)
        ((char*)literal)[str_len] = '\0';

    t->tokens[t->n_tokens] = TokenInit(type, lexeme, literal, t->line);

    if (type == TOKEN_STRING)
        ((char*)literal)[str_len] = '"';

    t->n_tokens++;
    free(lexeme);
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

    addToken(t, TOKEN_STRING, &t->source[t->start+1], t->current - t->start - 2);
}

static void number(Tokenizer *t) {
    while (isDigit(peek(t)))
        readChar(t);

    if (peek(t) == '.' && isDigit(peekNext(t))) {
        readChar(t);
        while (isDigit(peek(t)))
            readChar(t);
    }

    double f = atof(&t->source[t->start]);
    addToken(t, TOKEN_NUMBER, &f, 0);
}

static void identifier(Tokenizer *t) {
    while (isAlphaNum(peek(t)))
        readChar(t);

    addToken(t, TOKEN_IDENTIFIER, NULL, 0);
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
        .n_tokens = 0
    };

    for (int i = 0; i < MAX_TOKENS; i++)
        memset(&t.tokens[i], 0x00, sizeof(Token));

    return t;
}

void TokenizerFini(Tokenizer *t) {
    for (size_t i = 0; i < t->n_tokens; i++)
        TokenFini(&t->tokens[i]);

    free(t->source);
}

void TokenizerGetToken(Tokenizer *t) {
    char c = readChar(t);

    switch (c) {
    case '(': addToken(t, TOKEN_LEFT_PAREN, NULL, 0); break;
    case ')': addToken(t, TOKEN_RIGHT_PAREN, NULL, 0); break;
    case '{': addToken(t, TOKEN_LEFT_BRACE, NULL, 0); break;
    case '}': addToken(t, TOKEN_RIGHT_BRACE, NULL, 0); break;
    case ',': addToken(t, TOKEN_COMMA, NULL, 0); break;
    case '.': addToken(t, TOKEN_DOT, NULL, 0); break;
    case '-': addToken(t, TOKEN_MINUS, NULL, 0); break;
    case '+': addToken(t, TOKEN_PLUS, NULL, 0); break;
    case ';': addToken(t, TOKEN_SEMICOLON, NULL, 0); break;
    case '*': addToken(t, TOKEN_STAR, NULL, 0); break;
    case '"': string(t); break;
    case '=':
        addToken(t, match(t, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL, NULL, 0);
        break;

    case '!':
        addToken(t, match(t, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG, NULL, 0);
        break;

    case '<':
        addToken(t, match(t, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS, NULL, 0);
        break;

    case '>':
        addToken(t, match(t, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER, NULL, 0);
        break;

    case '/':
        if (match(t, '/')) {
            skipSingleline(t);
        } else if (match(t, '*')) {
            skipMultiline(t);
        } else {
            addToken(t, TOKEN_SLASH, NULL, 0);
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

const Token *TokenizerGetAllTokens(Tokenizer *t, size_t *n_len) {
    while (!isAtEnd(t)) {
        t->start = t->current;
        TokenizerGetToken(t);
    }

    t->tokens[t->n_tokens] = TokenInit(TOKEN_EOF, NULL, NULL, t->line);
    t->n_tokens++;
    *n_len = t->n_tokens;

    return t->tokens;
}
