#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "lexer.h"
#include "logging.h"

/* ---- HELPER FUNCTIONS ---- */

static inline bool __isWhitespace(char c);
static inline bool __isDigit(char c);
static inline bool __isAlpha(char c);
static inline bool __isAlphaNum(char c);
static char __peek(const Lexer *t);
static char __peekNext(const Lexer *t);
static char __advance(Lexer *t);
static Token __createToken(const Lexer *t, TokenType type);
static bool __match(Lexer *t, char expected);
static void __skipMultiline(Lexer *t);
static void __skipWhitespace(Lexer *t);
static void __skipSingleline(Lexer *t);

static __Map __MapInit(void);
static void __MapFini(__Map *m);

static void __MapSet(__Map *m, const char *str, TokenType value);
static TokenType __MapGet(__Map *m, const char *key, size_t keylen, TokenType default_ret);

/* ---- MAIN DEFINITIONS ---- */

Token TokenInit(TokenType type, const char *lexeme, size_t lexeme_len) {
    return (Token){
        .type = type,
        .lexeme = lexeme,
        .lexeme_len = lexeme_len
    };
}

Lexer LexerInit(const char *str, size_t len) {
    Lexer retval = {
        .source = str,
        .source_len = len,
        .start = 0,
        .current = 0,
        .line = 1,
        .reserved = __MapInit()
    };

    __MapSet(&retval.reserved, "and",    TOKEN_AND);
    __MapSet(&retval.reserved, "class",  TOKEN_CLASS);
    __MapSet(&retval.reserved, "else",   TOKEN_ELSE);
    __MapSet(&retval.reserved, "false",  TOKEN_FALSE);
    __MapSet(&retval.reserved, "for",    TOKEN_FOR);
    __MapSet(&retval.reserved, "fun",    TOKEN_FUN);
    __MapSet(&retval.reserved, "if",     TOKEN_IF);
    __MapSet(&retval.reserved, "nil",    TOKEN_NIL);
    __MapSet(&retval.reserved, "or",     TOKEN_OR);
    __MapSet(&retval.reserved, "print",  TOKEN_PRINT);
    __MapSet(&retval.reserved, "return", TOKEN_RETURN);
    __MapSet(&retval.reserved, "super",  TOKEN_SUPER);
    __MapSet(&retval.reserved, "this",   TOKEN_THIS);
    __MapSet(&retval.reserved, "true",   TOKEN_TRUE);
    __MapSet(&retval.reserved, "var",    TOKEN_VAR);
    __MapSet(&retval.reserved, "while",  TOKEN_WHILE);

    return retval;
}

void LexerFini(Lexer *l) {
    __MapFini(&l->reserved);
}

bool LexerIsDone(const Lexer *l) {
    return l->current >= l->source_len;
}

Token LexerGetToken(Lexer *t) {
    __skipWhitespace(t);
    if (LexerIsDone(t))
        return TokenEOF;
    
    t->start = t->current;
    char c = __advance(t);
    
    switch (c) {
    case '(': return __createToken(t, TOKEN_LEFT_PAREN);
    case ')': return __createToken(t, TOKEN_RIGHT_PAREN);
    case '{': return __createToken(t, TOKEN_LEFT_BRACE);
    case '}': return __createToken(t, TOKEN_RIGHT_BRACE);
    case ',': return __createToken(t, TOKEN_COMMA);
    case '.': return __createToken(t, TOKEN_DOT);
    case '-': return __createToken(t, TOKEN_MINUS);
    case '+': return __createToken(t, TOKEN_PLUS);
    case ';': return __createToken(t, TOKEN_SEMICOLON);
    case ':': return __createToken(t, TOKEN_COLON);
    case '?': return __createToken(t, TOKEN_QUESTION);
    case '=':
        return __createToken(t, __match(t, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    
    case '!':
        return __createToken(t, __match(t, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    
    case '<':
        return __createToken(t, __match(t, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    
    case '>':
        return __createToken(t, __match(t, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

    case '*':
        if (__peek(t) == '/') {
            error(t->line, "Dangling multiline comment end.\n");
            return TokenIllegal;
        }
        return __createToken(t, TOKEN_STAR);
    
    case '"':
        while (__peek(t) != '"' && !LexerIsDone(t)) {
            if (__peek(t) == '\n')
                t->line++;
            __advance(t);
        }

        if (LexerIsDone(t)) {
            error(t->line, "Unterminated string.\n");
            return TokenIllegal;
        }

        __advance(t);

        return __createToken(t, TOKEN_STRING);
    
    case '/':
        if (__match(t, '/'))
            __skipSingleline(t);
        else if (__match(t, '*'))
            __skipMultiline(t);
        else
            return __createToken(t, TOKEN_SLASH);

        t->start = t->current;
        if (LexerIsDone(t))
            break;
        
        return LexerGetToken(t);

    default:
        break;
    }

    if (__isDigit(c)) {
        while (__isDigit(__peek(t)))
            __advance(t);

        if (__peek(t) == '.' && __isDigit(__peekNext(t))) {
            do {
                __advance(t);
            } while (__isDigit(__peek(t)));
        }

        if (__isAlpha(__peek(t))) {
            error(t->line, "Numeric literal cannot be followed by an underscore or alphabetical character.\n");
            return TokenIllegal;
        }

        return __createToken(t, TOKEN_NUMBER);
    } else if (__isAlpha(c)) {
        TokenType type;

        while (__isAlphaNum(__peek(t)))
            __advance(t);

        type = __MapGet(&t->reserved, &t->source[t->start], t->current - t->start, TOKEN_IDENTIFIER);

        return __createToken(t, type);
    }

    error(t->line, "Unexpected character.\n");
    return TokenIllegal;
}

static inline bool __isWhitespace(const char c) {
    return c == ' ' || c == '\r' || c == '\t' || c == '\n';
}

static inline bool __isDigit(const char c) {
    return c <= '9' && c >= '0';
}

static inline bool __isAlpha(const char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'z') ||
           (c == '_');
}

static inline bool __isAlphaNum(const char c) {
    return __isAlpha(c) || __isDigit(c);
}

static char __peek(const Lexer *t) {
    if (LexerIsDone(t))
        return '\0';
    
    return t->source[t->current];
}

static char __peekNext(const Lexer *t) {
    if (t->current + 1 >= t->source_len)
        return '\0';

    return t->source[t->current + 1];
}

static char __advance(Lexer *t) {
    char c = __peek(t);
    if (c != '\0')
        t->current++;
    else if (c == '\n')
        t->line++;

    return c;
}

static Token __createToken(const Lexer *t, TokenType type) {
    return TokenInit(type, &t->source[t->start], t->current - t->start);
}

static bool __match(Lexer *t, char expected) {
    if (LexerIsDone(t) || expected != t->source[t->current])
        return false;

    t->current++;
    return true;
}

static void __skipMultiline(Lexer *t) {
    while (!LexerIsDone(t)) {
        char c = __advance(t);
        if (c != '*')
            continue;

        if (__peek(t) == '/') {
            __advance(t);
            return;
        }
    }    

    error(t->line, "Unmatched multiline comment.\n");
}

static void __skipSingleline(Lexer *t) {
    while (__peek(t) != '\n' && LexerIsDone(t))
        __advance(t);
    
    if (__peek(t) == '\n')
        t->line++;
}

static void __skipWhitespace(Lexer *t) {
    while (__isWhitespace(__peek(t)) && !LexerIsDone(t))
        __advance(t);
}

static inline int __hash(const char *str, size_t len) {
    return str[0] + str[len - 1] + len;
}

static __Entry *__pair(const char *key, TokenType value) {
    __Entry *retval = malloc(sizeof(__Entry));
    *retval = (__Entry){
        .key = strdup(key),
        .value = value,
        .next = NULL
    };

    return retval;
}

static __Map __MapInit(void) {
    __Map retval;
    for (int i = 0; i < TABLE_SIZE; i++)
        retval.buckets[i] = NULL;

    return retval;
}

static void __MapFini(__Map *m) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (m->buckets[i] == NULL)
            continue;

        for (__Entry *iter = m->buckets[i], *next = NULL; iter != NULL; iter = next) {
            next = iter->next;
            free(iter->key);
            free(iter);
        }
    }
}

static void __MapSet(__Map *m, const char *key, TokenType value) {
    int i = __hash(key, strlen(key)) % TABLE_SIZE;

    if (m->buckets[i] == NULL) {
        m->buckets[i] = __pair(key, value);
        return;
    }

    __Entry *iter;
    for (iter = m->buckets[i]; iter->next != NULL; iter = iter->next) {
        if (strcmp(key, iter->key) == 0) {
            iter->value = value;
            return;
        }
    }

    iter->next = __pair(key, value);
}

static TokenType __MapGet(__Map *m, const char *key, size_t keysize, TokenType default_ret) {
    int i = __hash(key, keysize) % TABLE_SIZE;

    for (__Entry *iter = m->buckets[i]; iter != NULL; iter = iter->next) {
        if (strlen(iter->key) != keysize)
            continue;

        if (strncmp(key, iter->key, keysize) == 0)
            return iter->value;
    }

    return default_ret;
}

