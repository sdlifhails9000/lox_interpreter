#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>

#include "logging.h"
#include "parser.h"

/* ---- HELPER FUNCTIONS ---- */

static Object createObject(enum ObjectType type, void *value) {
    Object retval = { .type = type };

    switch (type) {
    case OBJECT_BOOL:
        retval.value.b = *(bool*)value;
        break;
    case OBJECT_NUM:
        retval.value.f = *(double*)value;
        break;
    case OBJECT_STR:
        retval.value.str = ToStrView(value);
        break;
    default:
        break;
    }

    return retval;
}

static inline Token previous(Parser *p) {
    return p->tokens->array[p->current - 1];
}

static inline Token peek(Parser *p) {
    return p->tokens->array[p->current];
}

static inline bool isAtEnd(Parser *p) {
    return peek(p).type == TOKEN_EOF;
}

static inline Token readToken(Parser *p) {
    if (!isAtEnd(p))
        p->current++;

    return previous(p);
}

static inline bool check_type(Parser *p, TokenType type) {
    if (isAtEnd(p))
        return false;

    return peek(p).type == type;
}

static void synchronise(Parser *p) {
    readToken(p);
    
    while (!isAtEnd(p)) {
        if (previous(p).type == TOKEN_SEMICOLON)
            return;

        switch (peek(p).type) {
        case TOKEN_CLASS:
        case TOKEN_FUN:
        case TOKEN_VAR:
        case TOKEN_FOR:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_PRINT:
        case TOKEN_RETURN:
            return;
        default:
            break;
        }

        readToken(p);
    }
}

static void parser_error(Token token, const char *msg) {
    error1(token, msg);
}

static int consume(Parser *p, TokenType type, const char *msg, Token *out) {
    if (check_type(p, type)) {
        *out = readToken(p);
        return 0;
    }
    
    parser_error(peek(p), msg);
    return -1;
}

static bool match(Parser *p, int n, ...) {
    assert(n > 0);

    va_list types;
    va_start(types, n);

    for (int i = 0; i < n; i++) {
        TokenType type = va_arg(types, TokenType);
        if (check_type(p, type)) {
            readToken(p);
            va_end(types);
            return true;
        }
    }

    va_end(types);
    return false;
}

static Expr *tertiary(Parser *p);

static Expr *primary(Parser *p) {
    if (match(p, 1, TOKEN_FALSE)) {
        bool b = false;
        return (Expr*)LiteralInit(createObject(OBJECT_BOOL, &b));
    } else if (match(p, 1, TOKEN_TRUE)) {
        bool b = true;
        return (Expr*)LiteralInit(createObject(OBJECT_BOOL, &b));
    } else if (match(p, 1, TOKEN_NIL)) {
        return (Expr*)LiteralInit(createObject(OBJECT_NIL, NULL));
    } else if (match(p, 1, TOKEN_STRING)) {
        Str str = previous(p).literal.str;
        return (Expr*)LiteralInit(createObject(OBJECT_STR, &str));
    } else if (match(p, 1, TOKEN_NUMBER)) {
        double f = previous(p).literal.f;
        return (Expr*)LiteralInit(createObject(OBJECT_NUM, &f));
    } else if (match(p, 1, TOKEN_LEFT_PAREN)) {
        Expr *expr = tertiary(p);
        if (expr == NULL)
            return NULL;

        Token rightParen;
        if (consume(p, TOKEN_RIGHT_PAREN, "Expected ')' after expression.\n", &rightParen) != 0) {
            expr->fini(expr);
            return NULL;
        }

        return (Expr*)GroupingInit(expr);
    }

    parser_error(previous(p), "Expected literal.\n");
    return NULL;
}


static Expr *unary(Parser *p) {
    if (match(p, 2, TOKEN_BANG, TOKEN_MINUS, TOKEN_PLUS)) {
        if (previous(p).type == TOKEN_PLUS) {
            parser_error(previous(p), "Invalid unary plus");
            return NULL;
        }

        Token operator = previous(p);
        Expr *right = unary(p);
        if (right == NULL)
            return NULL;

        return (Expr*)UnaryInit(operator, right);
    }

    return primary(p);
}

static Expr *factor(Parser *p) {
    Expr *expr = unary(p);
    if (expr == NULL)
        return NULL;

    while (match(p, 2, TOKEN_SLASH, TOKEN_STAR)) {
        Token operator = previous(p);
        Expr *right = unary(p);
        if (right == NULL) {
            expr->fini(expr);
            return NULL;
        }

        expr = (Expr*)BinaryInit(expr, operator, right);
    }

    return expr;
}

static Expr *term(Parser *p) {
    Expr *expr = factor(p);
    if (expr == NULL)
        return NULL;

    while (match(p, 2, TOKEN_PLUS, TOKEN_MINUS)) {
        Token operator = previous(p);
        Expr *right = factor(p);
        if (right == NULL) {
            expr->fini(expr);
            return NULL;
        }

        expr = (Expr*)BinaryInit(expr, operator, right);
    }

    return expr;
}

static Expr *comparison(Parser *p) {
    Expr *expr = term(p);
    if (expr == NULL)
        return NULL;

    while (match(p, 4, TOKEN_LESS, TOKEN_LESS_EQUAL,
                 TOKEN_GREATER, TOKEN_GREATER_EQUAL)) {
        Token operator = previous(p);
        Expr *right = term(p);
        if (right == NULL) {
            expr->fini(expr);
            return NULL;
        }

        expr = (Expr*)BinaryInit(expr, operator, right);
    }

    return expr;
}

static Expr *equality(Parser *p) {
    Expr *expr = comparison(p);
    if (expr == NULL)
        return NULL;

    while (match(p, 2, TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL)) {
        Token operator = previous(p);
        Expr *right = comparison(p);
        if (right == NULL) {
            expr->fini(expr);
            return NULL;
        }

        expr = (Expr*)BinaryInit(expr, operator, right);
    }

    return expr;
}

static Expr *expression(Parser *p) {
    if (match(p, 8, TOKEN_SLASH, TOKEN_STAR, TOKEN_COMMA,
              TOKEN_LESS, TOKEN_LESS_EQUAL,
              TOKEN_GREATER, TOKEN_GREATER_EQUAL,
              TOKEN_EQUAL_EQUAL)) {
        parser_error(previous(p), "Missing left expression.\n");
    }

    Expr *expr = equality(p);
    if (expr == NULL)
        return NULL;

    while (match(p, 1, TOKEN_COMMA)) {
        Token comma = previous(p);
        Expr *right = equality(p);
        if (right == NULL) {
            expr->fini(expr);
            return NULL;
        }

        expr = (Expr*)BinaryInit(expr, comma, right);
    }
    
    return expr;
}

static Expr *tertiary(Parser *p) {
    Expr *condition = expression(p);
    if (condition == NULL)
        return NULL;

    if (!match(p, 1, TOKEN_QUESTION))
        return condition;

    Expr *ifTrue = expression(p);
    if (ifTrue == NULL) {
        condition->fini(condition);
        return NULL;
    }

    if (match(p, 1, TOKEN_COLON)) {
        Expr *ifFalse = expression(p);
        if (ifFalse != NULL)
            return (Expr*)TertiaryInit(condition, ifTrue, ifFalse);

        condition->fini(condition);
        ifTrue->fini(ifTrue);
        return NULL;
    }

    parser_error(peek(p), "Missing colon for the tertiary operator.\n");
    ifTrue->fini(ifTrue);
    condition->fini(condition);
    return NULL;
}

/* ---- MAIN METHODS ---- */

Parser ParserInit(TokenArray *tokens) {
    return (Parser){ .tokens = tokens, .current = 0 };
}

Expr *ParserParse(Parser *p) {
    Expr *result = tertiary(p);
    if (hadError) {
        result->fini(result);
        return NULL;
    }

    return result;
}

