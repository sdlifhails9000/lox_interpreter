#ifndef EXPR_H_
#define EXPR_H_

#include "token.h"

typedef struct Visitor Visitor;
typedef struct Expr Expr;

struct Expr {
    void *(*accept)(Visitor *v, Expr *expr);
    void (*fini)(Expr *expr);
};

typedef struct {
    Expr base;
    Expr *left;
    Token operator;
    Expr *right;
} Binary;

typedef struct {
    Expr base;
    Token operator;
    Expr *right;
} Unary;

typedef struct {
    Expr base;
    Expr *expr;
} Grouping;

typedef struct {
    Expr base;
    Token literal;
} Literal;

Binary *BinaryInit(Expr *left, Token operator, Expr *right);
Unary *UnaryInit(Token operator, Expr *right);
Grouping *GroupingInit(Expr *expr);
Literal *LiteralInit(Token literal);

void BinaryFini(Expr *b);
void UnaryFini(Expr *u);
void GroupingFini(Expr *g);
void LiteralFini(Expr *l);

struct Visitor {
    void *(*visitBinaryExpr)(Visitor *v, Binary *b);
    void *(*visitGroupingExpr)(Visitor *v, Grouping *g);
    void *(*visitLiteralExpr)(Visitor *v, Literal *l);
    void *(*visitUnaryExpr)(Visitor *v, Unary *u);
};

#endif

