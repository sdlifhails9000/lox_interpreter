#ifndef AST_PRINTER_H_
#define AST_PRINTER_H_

#include "expr.h"

typedef struct {
    ExprVisitor base;
} AstPrinter;

AstPrinter AstPrinterInit();

void AstPrint(AstPrinter *a, Expr *e);

#endif
