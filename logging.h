#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdbool.h>

extern bool hadError;

typedef enum {
    LEVEL_INFO,
    LEVEL_WARN,
    LEVEL_ERROR
} ReportLevel;

void report(ReportLevel level, const char *where, int line, const char *msg);
void error(int line, const char *msg);

#endif

