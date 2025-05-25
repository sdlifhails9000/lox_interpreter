#include <stdlib.h>
#include <string.h>

#include "map.h"

static inline int hash(const char *str) {
    int len = strlen(str);
    return str[0] + str[len-1] + len;
}

static Entry *pair(const char *key, TokenType value) {
    Entry *retval = malloc(sizeof(Entry));
    *retval = (Entry){
        .key = key,
        .value = value,
        .next = NULL
    };

    return retval;
}

Map MapInit(void) {
    Map retval;

    for (int i = 0; i < TABLE_SIZE; i++)
        retval.table[i] = NULL;

    return retval;
}

void MapFini(Map *m) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (m->table[i] == NULL)
            continue;

        for (Entry *iter = m->table[i], *next = NULL; iter != NULL; iter = next) {
            next = iter->next;
            free(iter);
        }
    }
}

void MapSet(Map *m, const char *key, TokenType value) {
    int i = hash(key) % TABLE_SIZE;

    if (m->table[i] == NULL) {
        m->table[i] = pair(key, value);
        return;
    }

    Entry *iter;
    for (iter = m->table[i]; iter->next != NULL; iter = iter->next) {
        if (strcmp(key, iter->key) == 0) {
            iter->key = value;
            iter->value = value;
            return;
        }
    }

    iter->next = pair(key, value);
}

TokenType MapGet(Map *m, const char *key) {
    int i = hash(key) % TABLE_SIZE;

    for (Entry *iter = m->table[i]; iter != NULL; iter = iter->next) {
        if (strcmp(key, iter->key) == 0)
            return iter->value;
    }

    return ~(unsigned int)0;
}

