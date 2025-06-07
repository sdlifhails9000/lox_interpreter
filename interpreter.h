#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include "expr.h"

typedef ExprVisitor Interpreter;

Interpreter InterpreterInit();
Object *InterpreterInterpret(Interpreter *i, Expr *e);

#endif
