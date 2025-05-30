#ifndef STR_H_
#define STR_H_

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

typedef struct {
    const char *str;
    size_t len;
} StrView;

typedef struct {
    char *str;
    size_t len;
    size_t max_len;
} Str;

Str StrInit(const char *str);
void StrFini(Str *s);

Str StrEmptyInit(size_t max_len);

Str StrSlice(Str *s, size_t i, size_t j);
StrView StrConstSlice(Str *s, size_t i, size_t j);

inline size_t StrLen(Str *str) {
    return str->len;
}

inline const char *StrViewToConstCStr(StrView *str) {
    char *retval = malloc((str->len + 1) * sizeof(char));
    strncpy(retval, str->str, str->len);
    return (const char*)retval;
}

StrView ToStrView(const char *str);

int StrCmp(Str *a, Str *b);
int StrCpy(Str *src, Str *dest);
int StrNCpy(Str *src, Str *dest, size_t n);

#endif

