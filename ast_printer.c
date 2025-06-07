#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "ast_printer.h"

void parenthesize(ExprVisitor *v, const char *name, int n, ...) {
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

/* ---- ExprVisitorS ---- */

void *visitTertiaryExpr(ExprVisitor *v, Tertiary *t) {
    parenthesize(v, "?:", 3, t->condition, t->ifTrue, t->ifFalse);
    return NULL;
}

void *visitBinaryExpr(ExprVisitor *v, Binary *b) {
    char lexeme[3] = {0};

    switch (b->operation) {
    case OPER_ADD:
        lexeme[0] = '+';
        break;
    case OPER_SUB:
        lexeme[0] = '-';
        break;
    case OPER_MUL:
        lexeme[0] = '*';
        break;
    case OPER_DIV:
        lexeme[0] = '/';
        break;

    case OPER_LESS_EQUAL:
        lexeme[1] = '=';
    case OPER_LESS:
        lexeme[0] = '<';
        break;
    
    case OPER_GREATER_EQUAL:
        lexeme[1] = '=';
    case OPER_GREATER:
        lexeme[0] = '>';
        break;

    case OPER_EQUAL:
        lexeme[0] = lexeme[1] = '=';
        break;

    default:
        break;
    }

    parenthesize(v, lexeme, 2, b->left, b->right);

    return NULL;
}

void *visitUnaryExpr(ExprVisitor *v, Unary *u) {
    char lexeme[1] = {0};

    switch (u->operation) {
    case OPER_BOOL_NOT:
        lexeme[0] = '!';
        break;
    case OPER_NEGATE:
        lexeme[0] = '-';
        break;
    default:
        break;
    }

    parenthesize(v, lexeme, 1, u->right);
    
    return NULL;
}

void *visitGroupingExpr(ExprVisitor *v, Grouping *g) {
    parenthesize(v, "group", 1, g->expr);
    return NULL;
}

void *visitLiteralExpr(__attribute__((unused)) ExprVisitor *v, Literal *l) {
    Object *obj = l->object;
    switch (obj->type) {
    case OBJECT_BOOL:
        printf("%s", obj->value.b ? "true" : "false");
        break;
    case OBJECT_NIL:
        printf("nil");
        break;
    case OBJECT_NUMBER:
        printf("%.3lf", obj->value.f);
        break;
    case OBJECT_STRING:
        printf("'%s'", obj->value.str);
    }

    return NULL;
}

/* ---- MAIN METHODS ---- */

AstPrinter AstPrinterInit() {
    return (AstPrinter){
        .base = (ExprVisitor){
            .visitTertiaryExpr = visitTertiaryExpr,
            .visitBinaryExpr = visitBinaryExpr,
            .visitUnaryExpr = visitUnaryExpr,
            .visitGroupingExpr = visitGroupingExpr,
            .visitLiteralExpr = visitLiteralExpr,
        }
    };
}

void AstPrint(AstPrinter *a, Expr *e) {
    e->accept((ExprVisitor*)a, e);
    putchar('\n');
}
