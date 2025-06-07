#ifndef OBJECT_H_
#define OBJECT_H_

#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    OBJECT_NUMBER,
    OBJECT_BOOL,
    OBJECT_STRING,
    OBJECT_NIL
} ObjectType;

typedef struct {
    ObjectType type;
    union {
        bool b;
        double f;
        char *str;
    } value;
} Object;

static inline Object *ObjectStr(const char *value, size_t len) {
    Object *retval = malloc(sizeof(Object));

    *retval = (Object){ .type = OBJECT_STRING, .value.str = malloc(len + 1) };
    retval->value.str[len] = '\0';

    for (size_t i = 0; i < len; i++)
        retval->value.str[i] = value[i];

    return retval; 
}

static inline Object *ObjectBool(bool value) {
    Object *retval = malloc(sizeof(Object));
    *retval = (Object){ .type = OBJECT_BOOL, .value.b = value };
    return retval;    
}

static inline Object *ObjectNum(double value) {
    Object *retval = malloc(sizeof(Object));
    *retval = (Object){ .type = OBJECT_NUMBER, .value.f = value };
    return retval;    
}

static inline Object *ObjectNil() {
    Object *retval = malloc(sizeof(Object));
    *retval = (Object){ .type = OBJECT_NIL };
    return retval;    
}

static inline void ObjectFini(Object *obj) {
    if (obj->type == OBJECT_STRING)
        free(obj->value.str);
    free(obj);
}

#endif
