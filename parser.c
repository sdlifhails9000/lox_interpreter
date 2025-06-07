#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include "object.h"
#include "logging.h"
#include "parser.h"

/* ---- HELPER FUNCTIONS ---- */

static inline const Token *previous(Parser *p) {
    return &p->tokens[p->current - 1];
}

static inline const Token *peek(Parser *p) {
    return &p->tokens[p->current];
}

static inline bool isAtEnd(Parser *p) {
    return peek(p)->type == TOKEN_EOF;
}

static inline const Token *readToken(Parser *p) {
    if (!isAtEnd(p))
        p->current++;

    return previous(p);
}

static inline bool check_type(Parser *p, TokenType type) {
    if (isAtEnd(p))
        return false;

    return peek(p)->type == type;
}

static void synchronise(Parser *p) {
    readToken(p);
    
    while (!isAtEnd(p)) {
        if (previous(p)->type == TOKEN_SEMICOLON)
            return;

        switch (peek(p)->type) {
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

static void parser_error(const Token *token, const char *msg) {
    error1(*token, msg);
}

static const Token *consume(Parser *p, TokenType type, const char *msg) {
    if (!check_type(p, type)) {
        parser_error(peek(p), msg);
        return NULL;
    }
    
    return readToken(p);
}

static bool match(Parser *p, int n, ...) {
    assert(n > 0);

    va_list types;
    va_start(types, n);

    for (int i = 0; i < n; i++) {
        TokenType type = va_arg(types, TokenType);
        if (check_type(p, type)) {
            va_end(types);
            readToken(p);
            return true;
        }
    }

    va_end(types);
    return false;
}

static Expr *tertiary(Parser *p);

static Expr *primary(Parser *p) {
    if (match(p, 1, TOKEN_TRUE)) {
        return (Expr*)LiteralInit(ObjectBool(true));
    } else if (match(p, 1, TOKEN_FALSE)) {
        return (Expr*)LiteralInit(ObjectBool(false));
    } else if (match(p, 1, TOKEN_NUMBER)) {
        char *str_num = malloc(previous(p)->lexeme_len + 1);
        double num = 0.0;

        str_num[previous(p)->lexeme_len] = '\0';

        strncpy(str_num, previous(p)->lexeme, previous(p)->lexeme_len);
        num = atof(str_num);
        free(str_num);
        
        return (Expr*)LiteralInit(ObjectNum(num));
    } else if (match(p, 1, TOKEN_STRING)) {
        return (Expr*)LiteralInit(ObjectStr(&previous(p)->lexeme[1], previous(p)->lexeme_len - 2));
    } else if (match(p, 1, TOKEN_NIL)) {
        return (Expr*)LiteralInit(ObjectNil());
    } else if (match(p, 1, TOKEN_LEFT_PAREN)) {
        Expr *expr = tertiary(p);
        if (expr == NULL)
            return NULL;

        if (consume(p, TOKEN_RIGHT_PAREN, "Expected ')' after expression.\n") == NULL) {
            ExprFini(expr);
            return NULL;
        }

        return (Expr*)GroupingInit(expr);
    }

    parser_error(peek(p), "Expected literal.\n");
    return NULL;
}


static Expr *unary(Parser *p) {
    if (match(p, 2, TOKEN_BANG, TOKEN_MINUS, TOKEN_PLUS)) {
        if (previous(p)->type == TOKEN_PLUS) {
            parser_error(previous(p), "Invalid unary plus");
            return NULL;
        }

        Operation oper;
        Expr *right;

        switch (previous(p)->type) {
        case TOKEN_BANG:
            oper = OPER_BOOL_NOT;
            break;
        case TOKEN_MINUS:
            oper = OPER_NEGATE;
            break;
        default:
            break;
        }
        
        right = unary(p);
        if (right == NULL)
            return NULL;

        return (Expr*)UnaryInit(oper, right);
    }

    return primary(p);
}

static Expr *factor(Parser *p) {
    Expr *expr = unary(p);
    if (expr == NULL)
        return NULL;

    while (match(p, 2, TOKEN_SLASH, TOKEN_STAR)) {
        Operation oper;
        Expr *right;

        switch (previous(p)->type) {
        case TOKEN_SLASH:
            oper = OPER_DIV;
            break;
        case TOKEN_STAR:
            oper = OPER_MUL;
            break;
        default:
            break;
        }
        
        right = unary(p);
        if (right == NULL) {
            ExprFini(expr);
            return NULL;
        }

        expr = (Expr*)BinaryInit(expr, oper, right);
    }

    return expr;
}

static Expr *term(Parser *p) {
    Expr *expr = factor(p);
    if (expr == NULL)
        return NULL;

    while (match(p, 2, TOKEN_PLUS, TOKEN_MINUS)) {
        Operation oper;
        Expr *right;

        switch (previous(p)->type) {
        case TOKEN_PLUS:
            oper = OPER_ADD;
            break;
        case TOKEN_MINUS:
            oper = OPER_SUB;
            break;
        default:
            break;
        }
        
        right = factor(p);
        if (right == NULL) {
            ExprFini(expr);
            return NULL;
        }
        
        expr = (Expr*)BinaryInit(expr, oper, right);
    }

    return expr;
}

static Expr *comparison(Parser *p) {
    Expr *expr = term(p);
    if (expr == NULL)
        return NULL;

    while (match(p, 4, TOKEN_LESS, TOKEN_LESS_EQUAL,
                 TOKEN_GREATER, TOKEN_GREATER_EQUAL)) {
        Operation oper;
        Expr *right;
        
        switch (previous(p)->type) {
        case TOKEN_LESS:
            oper = OPER_LESS;
            break;
        case TOKEN_LESS_EQUAL:
            oper = OPER_LESS_EQUAL;
            break;
        case TOKEN_GREATER:
            oper = OPER_GREATER;
            break;
        case TOKEN_GREATER_EQUAL:
            oper = OPER_GREATER_EQUAL;
            break;
        default:
            break;
        }

        right = term(p);
        if (hadError) {
            ExprFini(expr);
            return NULL;
        }

        expr = (Expr*)BinaryInit(expr, oper, right);
    }

    return expr;
}

static Expr *equality(Parser *p) {
    Expr *expr = comparison(p);
    if (expr == NULL)
        return NULL;

    while (match(p, 2, TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL)) {
        Operation oper;
        Expr *right;
        
        switch (previous(p)->type) {
        case TOKEN_BANG_EQUAL:
            oper = OPER_NOT_EQUAL;
            break;
        case TOKEN_EQUAL_EQUAL:
            oper = OPER_EQUAL;
            break;
        default:
            break;
        }

        right = comparison(p);
        if (right == NULL) {
            ExprFini(expr);
            return NULL;
        }

        expr = (Expr*)BinaryInit(expr, oper, right);
    }

    return expr;
}

static Expr *tertiary(Parser *p) {
    Expr *condition = equality(p);
    if (condition == NULL)
        return NULL;

    if (!match(p, 1, TOKEN_QUESTION))
        return condition;

    Expr *ifTrue = equality(p);
    if (ifTrue == NULL) {
        ExprFini(condition);
        return NULL;
    }

    if (match(p, 1, TOKEN_COLON)) {
        Expr *ifFalse = equality(p);
        if (ifFalse != NULL)
            return (Expr*)TertiaryInit(condition, ifTrue, ifFalse);

        ExprFini(condition);
        ExprFini(ifTrue);
        return NULL;
    }

    parser_error(peek(p), "Missing colon for the tertiary operator.\n");
    ExprFini(condition);
    ExprFini(ifTrue);
    return NULL;
}

static Expr *expression(Parser *p) {
    bool isWrong = false;

    if (match(p, 8, TOKEN_SLASH, TOKEN_STAR, TOKEN_COMMA,
              TOKEN_LESS, TOKEN_LESS_EQUAL,
              TOKEN_GREATER, TOKEN_GREATER_EQUAL,
              TOKEN_EQUAL_EQUAL)) {
        parser_error(previous(p), "Missing left expression.\n");
        isWrong = true;
    }

    Expr *expr = tertiary(p);
    if (expr == NULL)
        return NULL;

    while (match(p, 1, TOKEN_COMMA)) {
        Expr *right = tertiary(p);
        if (expr == NULL) {
            ExprFini(expr);
            return NULL;
        }

        expr = (Expr*)BinaryInit(expr, OPER_COMMA, right);
    }

    if (isWrong) {
        ExprFini(expr);
        return NULL;
    }

    return expr;
}


/* ---- MAIN METHODS ---- */

Parser ParserInit(const Token *tokens) {
    return (Parser){ .tokens = tokens, .current = 0 };
}

Expr *ParserParse(Parser *p) {
    Expr *result = tertiary(p);
    if (hadError)
        return NULL;

    return result;
}

