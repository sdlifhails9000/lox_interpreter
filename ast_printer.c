#include <stdio.h>
#include <stdarg.h>

#include "ast_printer.h"
#include "str.h"

void parenthesize(Visitor *v, const char *name, int n, ...) {
    va_list exprs;
    va_start(exprs, n);

    printf("(%s", name);
    
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
    parenthesize(v, "?:", 3, t->condition, t->ifTrue, t->ifFalse);
    return NULL;
}

void *visitBinaryExpr(Visitor *v, Binary *b) {
    char *lexeme = malloc(b->operator.lexeme_len + 1);
    lexeme[b->operator.lexeme_len] = '\0';
    strncpy(lexeme, b->operator.lexeme, b->operator.lexeme_len);
    parenthesize(v, lexeme, 2, b->left, b->right);
    free(lexeme);

    return NULL;
}

void *visitUnaryExpr(Visitor *v, Unary *u) {
    char *lexeme = malloc(u->operator.lexeme_len + 1);
    lexeme[u->operator.lexeme_len] = '\0';
    strncpy(lexeme, u->operator.lexeme, u->operator.lexeme_len);
    parenthesize(v, lexeme, 1, u->right);
    free(lexeme);
    
    return NULL;
}

void *visitGroupingExpr(Visitor *v, Grouping *g) {
    parenthesize(v, "group", 1, g->expr);
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
        printf("%s", obj.value.str);
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
