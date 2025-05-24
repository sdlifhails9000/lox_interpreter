#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "token.h"
#include "tokenizer.h"
#include "logging.h"

#define MAX_LINE_SIZE 100

static void run(const char *source, size_t size) {
    Tokenizer lexer = TokenizerInit(source, size);
    const TokenArray *tokens = TokenizerGetAllTokens(&lexer);

    for (size_t i = 0; i < tokens->len; i++)
        TokenPrint(&tokens->array[i]);

    putchar('\n');

    TokenizerFini(&lexer);
}

static int runFile(const char *path) {
    FILE *f = NULL;
    char *buffer = NULL;
    size_t size = 0;

    if ((f = fopen(path, "rb")) == NULL)
        return 1;
    
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = malloc(size * sizeof(char));
    fread(buffer, sizeof(char), size, f);

    run(buffer, size);
    
    if (hadError) {
        free(buffer);
        fclose(f);
        exit(64);
    }

    free(buffer);
    fclose(f);
    return 0;
}

static void runPrompt(void) {
    char *line = NULL;
    size_t n_maxread = MAX_LINE_SIZE;

    while (true) {
        printf(">> ");
        ssize_t n_read = getline(&line, &n_maxread, stdin);
        if (n_read <= 0) {
            free(line);
            break;
        }

        run(line, n_read);
        hadError = false;
        free(line);
        line = NULL;
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

