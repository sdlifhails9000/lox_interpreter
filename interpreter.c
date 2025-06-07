#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "logging.h"

/* ---- AUXILIARY FUNCTIONS ---- */

static Object *evaluate(ExprVisitor *v, Expr *expr) {
    return expr->accept(v, expr);
}

/* ---- ExprVisitorS (grammar rules) ---- */

static void *visitLiteralExpr(ExprVisitor *v, Literal *l) {
    return l->object;
}

static void *visitGroupingExpr(ExprVisitor *v, Grouping *g) {
    return evaluate(v, g->expr);
}

static void *visitUnaryExpr(ExprVisitor *v, Unary *u) {
    Object *obj = evaluate(v, u->right);
    if (hadError)
        return NULL;

    Object *retval = NULL;

    switch (u->operation) {
    case OPER_NEGATE:
        if (obj->type != OBJECT_BOOL) {
            error(1, "unary '-' expects a number.\n");
            free(obj);
            free(retval);
            return NULL;
        }

        retval = ObjectNum(-obj->value.f);
        break;
    case OPER_BOOL_NOT:
        if (obj->type != OBJECT_BOOL) {
            error(1, "'!' expects a boolean.\n");
            free(obj);
            free(retval);
            return NULL;
        }

        retval = ObjectBool(!obj->value.b);
        break;
    default:
        break;
    }

    free(obj);
    return retval;
}

static void *visitBinaryExpr(ExprVisitor *v, Binary *b) {
    Object *left = evaluate(v, b->left);
    if (left == NULL)
        return NULL;

    Object *right = evaluate(v, b->right);
    if (right == NULL) {
        ObjectFini(left);
        return NULL;
    }

    Object *retval = malloc(sizeof(Object));

    switch (b->operation) {
    case OPER_ADD:
        if (left->type == OBJECT_NUMBER && right->type == OBJECT_NUMBER) {
            retval = ObjectNum(left->value.f + right->value.f);
        } else if (left->type == OBJECT_STRING && right->type == OBJECT_STRING) {
            int n = strlen(left->value.str);
            int m = strlen(right->value.str);
            char *str = malloc(n + m + 1);

            str[0] = str[n + m] = '\0';

            strncat(str, left->value.str, n);
            strncat(str, right->value.str, m);
            retval = ObjectStr(str, n + m);
        } else {
            error(1, "'+' expects either two strings or two numbers.\n");
            free(left);
            free(right);
            free(retval);
            return NULL;
        }

        break;
    case OPER_SUB:
        if (left->type == OBJECT_NUMBER && left->type == OBJECT_NUMBER) {
            retval = ObjectNum(left->value.f - right->value.f);
        } else {
            error(1, "'+' expects either two strings or two numbers.\n");
            ObjectFini(left);
            ObjectFini(right);
            ObjectFini(retval);
            return NULL;
        }
        break;

    case OPER_MUL:
        if (left->type == OBJECT_NUMBER && left->type == OBJECT_NUMBER) {
            retval = ObjectNum(left->value.f * right->value.f);
        } else {
           error(1, "'*' expects numeric arguments");
           
        }

        /* TODO: Implement string duplication. */

        break;
    case OPER_COMMA:
        retval = right;
        break;
    case OPER_EQUAL:
        if (left->type != right->type)
            retval = ObjectBool(false);
        else if (left->type == OBJECT_NUMBER && right->type == OBJECT_NUMBER)
            retval = ObjectBool(left->value.f == right->value.f);
        else if (left->type == OBJECT_STRING && right->type == OBJECT_STRING)
            retval = ObjectBool(strcmp(left->value.str, right->value.str) == 0);
        break;

    case OPER_NOT_EQUAL:
        if (left->type != right->type)
            retval = ObjectBool(true);
        else if (left->type == OBJECT_NUMBER && right->type == OBJECT_NUMBER)
            retval = ObjectBool(left->value.f != right->value.f);
        else if (left->type == OBJECT_STRING && right->type == OBJECT_STRING)
            retval = ObjectBool(strcmp(left->value.str, right->value.str) != 0);
        break;

    case OPER_LESS:        
        retval = ObjectBool(left->value.f < right->value.f);
        break;

    case OPER_LESS_EQUAL:
        retval = ObjectBool(left->value.f <= right->value.f);
        break;

    case OPER_GREATER:
        retval = ObjectBool(left->value.f > right->value.f);
        break;

    case OPER_GREATER_EQUAL:
        retval = ObjectBool(left->value.f >= right->value.f);
        break;
    }

    free(left);
    free(right);
    return retval;
}

static void *visitTertiaryExpr(ExprVisitor *v, Tertiary *t) {
    Object *condition = evaluate(v, t->condition);
    Object *retval = NULL;

    if (condition->type != OBJECT_BOOL) {
        error(1, "Tertiary operator expects condition to be a boolean.\n");
        return NULL;
    }

    if (condition->value.b)
        retval = evaluate(v, t->ifTrue);
    else
        retval = evaluate(v, t->ifFalse);
    
    return retval; 
}

/* ---- MAIN METHODS ---- */

Interpreter InterpreterInit() {
    return (Interpreter){
        .visitBinaryExpr = visitBinaryExpr,
        .visitGroupingExpr = visitGroupingExpr,
        .visitLiteralExpr = visitLiteralExpr,
        .visitTertiaryExpr = visitTertiaryExpr,
        .visitUnaryExpr = visitUnaryExpr
    };
}

Object *InterpretInterpret(Interpreter *i, const Expr *expr) {
    return (Object*)expr->accept((ExprVisitor*)i, expr);
}

