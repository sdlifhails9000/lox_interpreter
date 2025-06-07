#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdbool.h>

#include "lexer.h"

extern bool hadError;

typedef enum {
    LEVEL_INFO,
    LEVEL_WARN,
    LEVEL_ERROR
} ReportLevel;

void report(ReportLevel level, const char *where, int line, const char *msg);
void error(int line, const char *msg);
void error1(Token token, const char *msg);

#endif

