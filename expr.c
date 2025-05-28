#include <stdlib.h>

#include "expr.h"

static void *binaryAccept(Visitor *v, Expr *expr) {
    return v->visitBinaryExpr(v, (Binary*)expr);
}

static void *unaryAccept(Visitor *v, Expr *expr) {
    return v->visitUnaryExpr(v, (Unary*)expr);
}

static void *groupingAccept(Visitor *v, Expr *expr) {
    return v->visitGroupingExpr(v, (Grouping*)expr);
}

static void *literalAccept(Visitor *v, Expr *expr) {
    return v->visitLiteralExpr(v, (Literal*)expr);
}

void BinaryFini(Expr *b) {
    Binary *_b = (Binary*)b;
    _b->left->fini(_b->left);
    _b->right->fini(_b->right);
    TokenFini(&_b->operator);
    free(b);
}

void UnaryFini(Expr *u) {
    Unary *_u = (Unary*)u;
    _u->right->fini(_u->right);
    TokenFini(&_u->operator);
    free(u);
}

void GroupingFini(Expr *g) {
    Grouping *_g = (Grouping*)g;
    _g->expr->fini(_g->expr);
    free(g);
}

void LiteralFini(Expr *l) {
    Literal *_l = (Literal*)l;
    TokenFini(&_l->literal);
    free(l);
}

Binary *BinaryInit(Expr *left, Token operator, Expr *right) {
    Binary *retval = malloc(sizeof(Binary));
    *retval = (Binary){
        .base.accept = binaryAccept,
        .base.fini = BinaryFini,
        .left = left,
        .operator = operator,
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

Literal *LiteralInit(Token literal) {
    Literal *retval = malloc(sizeof(Literal));
    *retval = (Literal){
        .base.accept = literalAccept,
        .base.fini = LiteralFini,
        .literal = literal
    };

    return retval;
}

Unary *UnaryInit(Token operator, Expr *right) {
    Unary *retval = malloc(sizeof(Unary));
    *retval = (Unary){
        .base.accept = unaryAccept,
        .base.fini = UnaryFini,
        .operator = operator,
        .right = right
    };

    return retval;
}

