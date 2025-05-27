#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "token.h"
#include "tokenizer.h"
#include "logging.h"
#include "expr.h"
#include "ast_printer.h"
#include "str.h"

#define MAX_LINE_SIZE 100

static void run(Str source) {
    Tokenizer lexer = TokenizerInit(source);
    const TokenArray *tokens = TokenizerGetAllTokens(&lexer);

    for (size_t i = 0; i < tokens->len; i++)
        TokenPrint(&tokens->array[i]);

    putchar('\n');

    TokenizerFini(&lexer);
}

static int runFile(const char *path) {
    FILE *f = NULL;
    Str buffer = {};
    size_t size = 0;

    if ((f = fopen(path, "rb")) == NULL)
        return 1;
    
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = StrEmptyInit(size);
    fread(&buffer.str, sizeof(char), size, f);

    run(buffer);
    
    if (hadError) {
        StrFini(&buffer);
        fclose(f);
        exit(64);
    }

    StrFini(&buffer);
    fclose(f);
    return 0;
}

static void runPrompt(void) {
    Str line = StrEmptyInit(MAX_LINE_SIZE);
    size_t n_maxread = MAX_LINE_SIZE;

    while (true) {
        printf(">> ");
        ssize_t n_read = getline(&line.str, &n_maxread, stdin);
        if (n_read <= 0) {
            StrFini(&line);
            break;
        }

        run(line);
        hadError = false;
    }
}

int main(int argc, char **argv) {
//    if (argc > 2) {
//        printf("Usage: loxc [script]\n");
//        return 1;
//    }
//
//    int status = 0;
//
//    if (argc == 2) {
//        if (runFile(argv[1]) == 1) {
//            perror("Error opening file");
//            status = 1;
//        }
//    } else {
//        runPrompt();
//    }
//
//    return status;
    AstPrinter p = AstPrinterInit();
    
    AstPrint(
        &p,
        BinaryInit(
            UnaryInit(
                (Token){ .lexeme = "-" },
                LiteralInit((Token){ .type = TOKEN_NUMBER, .literal.f = 123 })
            ),
            (Token){ .lexeme = "*" },
            GroupingInit(
                LiteralInit((Token){ .type = TOKEN_NUMBER, .literal.f = 45.67 })
            )
        )
    );

    return 0;
}

