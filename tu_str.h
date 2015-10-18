#ifndef TU_STR_H_
#define TU_STR_H_

#include <stddef.h>
#include <string.h>

#define TU_STR_VAL(tustr) (tustr)->val
#define TU_STR_LEN(tustr) (tustr)->len

#define ZEND_MM_ALIGNMENT 8
#define ZEND_MM_ALIGNMENT_MASK ~(ZEND_MM_ALIGNMENT - 1)
#define ZEND_MM_ALIGNED_SIZE(size)  (((size) + ZEND_MM_ALIGNMENT - 1) & ZEND_MM_ALIGNMENT_MASK)

#define _ZSTR_HEADER_SIZE XtOffsetOf(tu_string_t, val)
#define _ZSTR_STRUCT_SIZE(len) (_ZSTR_HEADER_SIZE + len + 1)

typedef struct _tu_string tu_string_t;

struct _tu_string {
    size_t len;
    char val[1];
};

static inline tu_string_t* tu_string_alloc(size_t len)
{
    tu_string_t* ret = (tu_string_t*) malloc(ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)));
    TU_STR_LEN(ret) = len;
    return ret;
}

static inline tu_string_t* tu_string_init(const char *str, size_t len)
{
    tu_string_t* ret = tu_string_alloc(len);

    memcpy(TU_STR_VAL(ret), str, len);
    TU_STR_VAL(ret)[len] = '\0';
    return ret;
}

static inline void tu_string_free(tu_string_t* str)
{
    free(str);
}

#endif  // TU_STR_H_
