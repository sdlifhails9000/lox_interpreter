#include <stdlib.h>

#include "expr.h"

static void *tertiaryAccept(ExprVisitor *v, Expr *expr) {
    return v->visitTertiaryExpr(v, (Tertiary*)expr);
}

static void *binaryAccept(ExprVisitor *v, Expr *expr) {
    return v->visitBinaryExpr(v, (Binary*)expr);
}

static void *unaryAccept(ExprVisitor *v, Expr *expr) {
    return v->visitUnaryExpr(v, (Unary*)expr);
}

static void *groupingAccept(ExprVisitor *v, Expr *expr) {
    return v->visitGroupingExpr(v, (Grouping*)expr);
}

static void *literalAccept(ExprVisitor *v, Expr *expr) {
    return v->visitLiteralExpr(v, (Literal*)expr);
}

void ExprFini(Expr *e) {
    e->fini(e);
}

void TertiaryFini(Expr *t) {
    Tertiary *_t = (Tertiary*)t;
    ExprFini(_t->condition);
    ExprFini(_t->ifTrue);
    ExprFini(_t->ifFalse);
    free(t);
}

void BinaryFini(Expr *b) {
    Binary *_b = (Binary*)b;
    ExprFini(_b->left);
    ExprFini(_b->right);
    free(b);
}

void UnaryFini(Expr *u) {
    ExprFini(((Unary*)u)->right);
    free(u);
}

void GroupingFini(Expr *g) {
    ExprFini(((Grouping*)g)->expr);
    free(g);
}

void LiteralFini(Expr *l) {
    free(l);
}

Tertiary *TertiaryInit(Expr *condition, Expr *ifTrue, Expr *ifFalse) {
    Tertiary *retval = malloc(sizeof(Tertiary));
    *retval = (Tertiary){
        .base.accept = tertiaryAccept,
        .base.fini =  TertiaryFini,
        .condition = condition,
        .ifTrue = ifTrue,
        .ifFalse = ifFalse
    };

    return retval;
}

Binary *BinaryInit(Expr *left, Operation operator, Expr *right) {
    Binary *retval = malloc(sizeof(Binary));
    *retval = (Binary){
        .base.accept = binaryAccept,
        .base.fini = BinaryFini,
        .left = left,
        .operation = operator,
        .right = right
    };

    return retval;
}

Grouping *GroupingInit(Expr *expr) {
    Grouping *retval = malloc(sizeof(Grouping));
    *retval = (Grouping){
        .base.accept = groupingAccept,
        .base.fini = GroupingFini,
        .expr = expr
    };

    return retval;
}

Literal *LiteralInit(Object *object) {
    Literal *retval = malloc(sizeof(Literal));
    *retval = (Literal){
        .base.accept = literalAccept,
        .base.fini = LiteralFini,
        .object = object
    };

    return retval;
}

Unary *UnaryInit(Operation operator, Expr *right) {
    Unary *retval = malloc(sizeof(Unary));
    *retval = (Unary){
        .base.accept = unaryAccept,
        .base.fini = UnaryFini,
        .operation = operator,
        .right = right
    };

    return retval;
}

