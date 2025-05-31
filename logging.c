#include <stdio.h>
#include <stdlib.h>

#include "logging.h"
#include "token.h"

bool hadError = false;

void report(ReportLevel level, const char *where, int line, const char *msg) {
    const char *str_level = NULL;

    switch (level) {
    case LEVEL_INFO: str_level = "INFO"; break;
    case LEVEL_WARN: str_level = "WARNING"; break;
    case LEVEL_ERROR: str_level = "ERROR"; break;
    }

    fprintf(stderr, "[%s @ line %d] %s: %s", str_level, line, where, msg);
}

void error(int line, const char *msg) {
    report(LEVEL_ERROR, NULL, line, msg);
    hadError = true;
}

void error1(Token token, const char *msg) {
    if (token.type == TOKEN_EOF) {
        report(LEVEL_ERROR, "At the end", token.line, msg);
    } else {
        char where[50] = {0};
        const char *lexeme = StrViewToConstCStr(&token.lexeme);
        snprintf(where, sizeof(where), "At '%s'", lexeme);
        free(lexeme);
        report(LEVEL_ERROR, where, token.line, msg);
    }

    hadError = true;
}

