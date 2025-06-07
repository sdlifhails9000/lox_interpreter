#include <stdio.h>
#include <string.h>

#include "logging.h"

bool hadError = false;

void report(ReportLevel level, const char *where, int line, const char *msg) {
    const char *str_level = NULL;

    switch (level) {
    case LEVEL_INFO: str_level = "INFO"; break;
    case LEVEL_WARN: str_level = "WARNING"; break;
    case LEVEL_ERROR: str_level = "ERROR"; break;
    }

    if (where != NULL)
        fprintf(stderr, "[%s @ line %d] %s: %s", str_level, line, where, msg);
    else
        fprintf(stderr, "[%s @ line %d] %s", str_level, line, msg);        
}

void error(int line, const char *msg) {
    report(LEVEL_ERROR, NULL, line, msg);
    hadError = true;
}

void error1(Token token, const char *msg) {
    if (token.type == TOKEN_EOF) {
        report(LEVEL_ERROR, "At the end", token.line, msg);
    } else {
        char where[50] = "At '";
        strncat(where, token.lexeme, token.lexeme_len);
        strcat(where, "'");
        report(LEVEL_ERROR, where, token.line, msg);
    }

    hadError = true;
}

