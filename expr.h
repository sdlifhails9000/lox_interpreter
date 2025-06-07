#ifndef EXPR_H_
#define EXPR_H_

#include <stdbool.h>

#include "lexer.h"
#include "object.h"

typedef enum {
    OPER_ADD,
    OPER_SUB,
    OPER_MUL,
    OPER_DIV,
    OPER_COMMA,
    OPER_EQUAL,
    OPER_NOT_EQUAL,
    OPER_LESS,
    OPER_LESS_EQUAL,
    OPER_GREATER,
    OPER_GREATER_EQUAL,
    OPER_NEGATE,
    OPER_BOOL_NOT
} Operation;

typedef struct ExprVisitor ExprVisitor;

typedef struct  {
    void *(*accept)(ExprVisitor *v, Expr *expr);
    void (*fini)(Expr *expr);
    int line;
} Expr;

typedef struct {
    Expr base;
    Expr *left;
    Operation operation;
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
    Operation operation;
    Expr *right;
} Unary;

typedef struct {
    Expr base;
    Expr *expr;
} Grouping;

typedef struct {
    Expr base;
    Object *object;
} Literal;

Binary *BinaryInit(Expr *left, Operation oper, Expr *right);
Tertiary *TertiaryInit(Expr *condition, Expr *ifTrue, Expr *ifFalse);
Unary *UnaryInit(Operation oper, Expr *right);
Grouping *GroupingInit(Expr *expr);
Literal *LiteralInit(Object *object);

void ExprFini(Expr *e);
void TertiaryFini(Expr *t);
void BinaryFini(Expr *b);
void UnaryFini(Expr *u);
void GroupingFini(Expr *g);
void LiteralFini(Expr *l);

struct ExprVisitor {
    void *(*visitBinaryExpr)(ExprVisitor *v, Binary *b);
    void *(*visitTertiaryExpr)(ExprVisitor *v, Tertiary *t);
    void *(*visitGroupingExpr)(ExprVisitor *v, Grouping *g);
    void *(*visitLiteralExpr)(ExprVisitor *v, Literal *l);
    void *(*visitUnaryExpr)(ExprVisitor *v, Unary *u);
};

#endif
