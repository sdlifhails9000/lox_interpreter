#ifndef MAP_H_
#define MAP_H_

#include "token.h"

#define TABLE_SIZE 100

typedef struct {
    char *key;
    TokenType value;
    void *next;
} Entry;

typedef struct {
    Entry *table[TABLE_SIZE];
} Map;

Map MapInit(void);
void MapFini(Map *m);

void MapSet(Map *m, const char *str, TokenType value);
TokenType MapGet(Map *m, const char *key, TokenType default);

#endif

