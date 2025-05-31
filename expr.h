#ifndef EXPR_H_
#define EXPR_H_

#include <stdbool.h>
#include "token.h"

typedef struct {
    enum ObjectType {
        OBJECT_NUM = 0,
        OBJECT_BOOL,
        OBJECT_STR,
        OBJECT_NIL
    } type;
    union {
        bool b;
        double f;
        char *str;
    } value;
} Object;

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
    Expr *condition;
    Expr *ifTrue;
    Expr *ifFalse;
} Tertiary;

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
    Object object;
} Literal;

Binary *BinaryInit(Expr *left, Token operator, Expr *right);
Tertiary *TertiaryInit(Expr *condition, Expr *ifTrue, Expr *ifFalse);
Unary *UnaryInit(Token operator, Expr *right);
Grouping *GroupingInit(Expr *expr);
Literal *LiteralInit(Object object);

void TertiaryFini(Expr *t);
void BinaryFini(Expr *b);
void UnaryFini(Expr *u);
void GroupingFini(Expr *g);
void LiteralFini(Expr *l);

struct Visitor {
    void *(*visitBinaryExpr)(Visitor *v, Binary *b);
    void *(*visitTertiaryExpr)(Visitor *v, Tertiary *t);
    void *(*visitGroupingExpr)(Visitor *v, Grouping *g);
    void *(*visitLiteralExpr)(Visitor *v, Literal *l);
    void *(*visitUnaryExpr)(Visitor *v, Unary *u);
};

#endif

