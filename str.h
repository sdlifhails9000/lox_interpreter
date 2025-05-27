#ifndef STR_H_
#define STR_H_

#include <string.h>
#include <stddef.h>

typedef struct {
    const char *str;
    size_t len;
} StrView;

typedef struct {
    char *str;
    size_t len;
} Str;

inline Str StrInit(const char *str) {
    return (Str){
        .str = strdup(str),
        .len = strlen(str)
    };
}

inline void StrFini(Str *s) {
    free(s->str);
}

Str StrSlice(Str *s, int i, int j);
StrView StrConstSlice(Str *s, int i, int j);

inline Str StrEmptyInit(int len) {
    return (Str){
        .str = calloc(len, sizeof(char)),
        .len = len
    };
}

inline StrView ToStrView(const char *str) {
    return (StrView){ .str = str, .len = strlen(str) };
}

int StrCmp(Str *a, Str *b);
void StrCpy(Str *src, Str *dest);
void StrNCpy(Str *src, Str *dest, int n);

#endif

