#include <stdio.h>

#include "logging.h"

bool hadError = false;

void report(ReportLevel level, const char *where, int line, const char *msg) {
    const char *str_level;

    switch (level) {
    case LEVEL_INFO: str_level = "INFO"; break;
    case LEVEL_WARN: str_level = "WARN"; break;
    case LEVEL_ERROR: str_level = "ERROR"; break;
    }

    fprintf(stderr, "[%s; %d] %s: %s", str_level, line, where, msg);
}

void error(int line, const char *msg) {
    report(LEVEL_ERROR, NULL, line, msg);
    hadError = true;
}

