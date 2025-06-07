#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

#include "logging.h"
#include "lexer.h"
#include "ast_printer.h"
#include "parser.h"
#include "interpreter.h"

#define MAX_TOKENS 4096
#define MAX_LINE_SIZE 100

static inline void print_str(const char *str, size_t len) {
    for (size_t i = 0; i < len; i++)
        putchar(str[i]);
}

static void print_token(const Token *t) {
    if (t->type == TOKEN_EOF) {
        printf("EOF\n");
        return;
    }

    printf("%d, '",  t->type);
    print_str(t->lexeme, t->lexeme_len);
    printf("'\n");
}

static int run(char *source, size_t len) {
    Lexer lexer; 
    Parser parser;
    Interpreter interpreter;
    Token tokens[MAX_TOKENS]; 
    Expr *result; 
    Object *value;

    lexer = LexerInit(source, len);
    for (int i = 0; !LexerIsDone(&lexer) && i < MAX_TOKENS; i++) {
        tokens[i] = LexerGetToken(&lexer);
        if (tokens[i].type == TOKEN_ILLEGAL) {
            LexerFini(&lexer);
            return -1;
        }
        
        print_token(&tokens[i]);
    }
    LexerFini(&lexer);

    parser = ParserInit(tokens);
    result = ParserParse(&parser);
    if (result == NULL)
        return -1;

    AstPrinter ast = AstPrinterInit();
    AstPrint(&ast, result);

    interpreter = InterpreterInit();
    value = InterpretInterpret(&interpreter, result);
    if (value == NULL)
        return -1;

    ExprFini(result);

    switch (value->type) {
    case OBJECT_NUMBER:
        printf("%.3f\n", value->value.f);
        break;
    case OBJECT_BOOL:
        printf("%s\n", value->value.b ? "true" : "false");
        break;
    case OBJECT_NIL:
        printf("nil\n");
        break;
    case OBJECT_STRING:
        printf("'%s'\n", value->value.str);
        break;
    }

    ObjectFini(value);

    return 0;
}

static int runFile(const char *path) {
    FILE *f;
    char *buffer;
    size_t size;
    int retval = 0;

    if ((f = fopen(path, "rb")) == NULL)
        return -1;
    
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);

    buffer = malloc(size + 1);
    fread(buffer, sizeof(char), size, f);

    run(buffer, size);    
    if (hadError)
        retval = -1;

    free(buffer);
    fclose(f);
    return retval;
}

static void runPrompt(void) {
    char *line = malloc(MAX_LINE_SIZE * sizeof(char));
    size_t n_maxread = MAX_LINE_SIZE;

    while (true) {
        printf(">> ");
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
        printf("Usage: lox [script]\n");
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

