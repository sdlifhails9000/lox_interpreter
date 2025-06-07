#ifndef STMT_H_
#define STMT_H_

#include <stdbool.h>

#include "expr.h"

typedef struct StmtExprVisitor StmtExprVisitor;

typedef struct {
    void *(*accept)(StmtExprVisitor *v, Stmt *expr);
    void (*fini)(Stmt *expr);
    int line;
} Stmt;

typedef struct {

} Print;

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
