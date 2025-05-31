#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

#include "str.h"
#include "logging.h"
#include "tokenizer.h"
#include "ast_printer.h"
#include "parser.h"

#define MAX_LINE_SIZE 100

static void run(char *source, size_t len) {
    Tokenizer lexer; 
    const TokenArray *tokens; 
    Parser parser; 
    Expr *result; 

    lexer = TokenizerInit(source, len);
    tokens = TokenizerGetAllTokens(&lexer);
    parser = ParserInit(tokens);
    result = ParserParse(&parser);

    if (result == NULL)
        goto end;

    AstPrinter printer = AstPrinterInit();
    AstPrint(&printer, result);

    result->fini(result);

end:
    TokenizerFini(&lexer);
}

static int runFile(const char *path) {
    FILE *f;
    char *buffer;
    size_t size;

    if ((f = fopen(path, "rb")) == NULL)
        return 1;
    
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = malloc(size + 1);
    fread(buffer, sizeof(char), size, f);

    run(buffer, size);
    
    if (hadError) {
        fclose(f);
        exit(64);
    }

    fclose(f);
    return 0;
}

static void runPrompt(void) {
    char *line = NULL;
    size_t n_maxread = MAX_LINE_SIZE;

    while (true) {
        printf(">> ");
        line = malloc(MAX_LINE_SIZE * sizeof(char));
        ssize_t n_read = getline(&line, &n_maxread, stdin);

        if (n_read <= 0) {
            printf("\nDone.\n");
            free(line);
            break;
        }

        run(line, n_read);
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

