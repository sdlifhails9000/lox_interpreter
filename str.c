#include <stdlib.h>

#include "str.h"

Str StrInit(const char *str) {
    size_t len = strlen(str);

    return (Str){
        .str = strdup(str),
        .len = len,
        .max_len = len
    };
}

void StrFini(Str *s) {
    free(s->str);
}

Str StrEmptyInit(size_t max_len) {
    return (Str){
        .str = calloc(max_len, sizeof(char)),
        .max_len = max_len,
        .len = 0
    };
}

StrView ToStrView(const char *str) {
    return (StrView){ .str = str, .len = strlen(str) };
}

int StrNCpy(Str *src, Str *dest, size_t n) {
    if (n > dest->len)
        return -1;

    for (size_t i = 0; i < n && i < dest->len; i++) {
        char c;
        if (i >= src->len)
            c = '\0';
        else
            c = src->str[i];

        dest->str[i] = c;
    }

    return 0;
}

int StrCpy(Str *src, Str *dest) {
    if (src->len > dest->len)
        return -1;

    StrNCpy(src, dest, src->len);
    return 0;
}

int StrCmp(Str *a, Str *b) {
    if (a->len != b->len)
        return -1;

    return strncmp(a->str, b->str, a->len);
}

Str StrSlice(Str *s, size_t i, size_t j) {
    if (i < 0 || j >= s->len)
        return (Str){ .str = NULL, .len = 0 };;

    const size_t len = j - i + 1;
    Str retval = StrEmptyInit(len);
    retval.len = len;

    strncpy(retval.str, &s->str[i], len);

    return retval;
}

StrView StrConstSlice(Str *s, size_t i, size_t j) {
    if (i < 0 || j >= s->len)
        return (StrView){ .str = NULL, .len = 0 };;

    return (StrView){
        .str = &s->str[i],
        .len = j - i + 1
    };
}

