#include <stdio.h>
#include <stdarg.h>

#include "ast_printer.h"
#include "str.h"

void print_str(StrView str) {
    for (size_t i = 0; i < str.len; i++)
        putchar(str.str[i]);
}

void parenthesize(Visitor *v, StrView str, int n, ...) {
    va_list exprs;
    va_start(exprs, n);

    putchar('(');
    print_str(str);
    
    for (int i = 0; i < n; i++) {
        Expr *expr = va_arg(exprs, Expr*);
        putchar(' ');
        expr->accept(v, expr);
    }

    putchar(')');
    va_end(exprs);
}

/* ---- VISITORS ---- */

void *visitTertiaryExpr(Visitor *v, Tertiary *t) {
    parenthesize(v, ToStrView("?:"), 3, t->condition, t->ifTrue, t->ifFalse);
    return NULL;
}

void *visitBinaryExpr(Visitor *v, Binary *b) {
    parenthesize(v, b->operator.lexeme, 2, b->left, b->right);
    return NULL;
}

void *visitUnaryExpr(Visitor *v, Unary *u) {
    parenthesize(v, u->operator.lexeme, 1, u->right);
    return NULL;
}

void *visitGroupingExpr(Visitor *v, Grouping *g) {
    parenthesize(v, ToStrView("group"), 1, g->expr);
    return NULL;
}

void *visitLiteralExpr(__attribute__((unused)) Visitor *v, Literal *l) {
    Object obj = l->object;
    switch (obj.type) {
    case OBJECT_BOOL:
        printf("%s", obj.value.b ? "true" : "false");
        break;
    case OBJECT_NIL:
        printf("nil");
        break;
    case OBJECT_NUM:
        printf("%.3lf", obj.value.f);
        break;
    case OBJECT_STR:
        print_str(obj.value.str);
    }

    return NULL;
}

/* ---- MAIN METHODS ---- */

AstPrinter AstPrinterInit() {
    return (AstPrinter){
        .base = (Visitor){
            .visitTertiaryExpr = visitTertiaryExpr,
            .visitBinaryExpr = visitBinaryExpr,
            .visitUnaryExpr = visitUnaryExpr,
            .visitGroupingExpr = visitGroupingExpr,
            .visitLiteralExpr = visitLiteralExpr,
        }
    };
}

void AstPrint(AstPrinter *a, Expr *e) {
    e->accept((Visitor*)a, e);
    putchar('\n');
}
