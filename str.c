#include <stdlib.h>

#include "str.h"

int StrNCpy(Str *src, Str *dest, int n) {
    if (n > dest->len)
        return -1;

    for (int i = 0; i < n && i < dest->len; i++) {
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
    if (src->len > src->dest)
        return -1;

    StrNCpy(src, dest, src->len);
    return 0;
}

int StrCmp(Str *a, Str *b) {
    if (a->len != b->len)
        return -1;

    return strcmp(a->str, b->str);
}

Str StrSlice(Str *s, int i, int j) {
    if (i < 0 || j >= s->len)
        return (Str){ .str = NULL, .len = 0 };;

    const int len = j - i + 1;
    Str retval = StrEmptyInit(len);

    strncpy(retval.str, s->str, len);

    return retval;
}

StrView StrConstSlice(Str *s, int i, int j) {
    if (i < 0 || j >= s->len)
        return (Str){ .str = NULL, .len = 0 };;

    return (StrView){
        .str = &s->str[i],
        .len = j - i + 1
    };
}

