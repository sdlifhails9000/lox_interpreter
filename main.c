#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "token.h"
#include "tokenizer.h"
#include "logging.h"
#include "expr.h"
#include "ast_printer.h"
#include "str.h"

#define MAX_LINE_SIZE 100

static void run(Str source) {
    Tokenizer lexer = TokenizerInit(source);
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    const TokenArray *tokens = TokenizerGetAllTokens(&lexer);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;

    printf("\nTime taken by the tokenizer: %.3f\n", elapsed_ms);

    TokenizerFini(&lexer);
}

static int runFile(const char *path) {
    FILE *f;
    Str buffer;
    size_t size;

    if ((f = fopen(path, "rb")) == NULL)
        return 1;
    
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = StrEmptyInit(size + 1);
    int n_read = fread(buffer.str, sizeof(char), size, f);
    buffer.len = n_read;

    run(buffer);
    
    if (hadError) {
        fclose(f);
        exit(64);
    }

    fclose(f);
    return 0;
}

static void runPrompt(void) {
    Str line;
    size_t n_maxread = MAX_LINE_SIZE;

    while (true) {
        printf(">> ");
        line = StrEmptyInit(MAX_LINE_SIZE);
        ssize_t n_read = getline(&line.str, &n_maxread, stdin);

        if (n_read <= 0) {
            StrFini(&line);
            printf("\nDone.\n");
            break;
        }

        line.len = n_read;
        run(line);
        hadError = false;
    }
}

int main(int argc, char **argv) {
    if (argc > 2) {
        printf("Usage: loxc [script]\n");
        return 1;
    }

    int status = 0;

    if (argc == 2) {
        if (runFile(argv[1]) == 1) {
            perror("Error opening file");
            status = 1;
        }
    } else {
        runPrompt();
    }

    return status;
}

