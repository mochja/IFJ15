/**
 * Copyright (c) 2015, Team Unknown,
 *                     Ján Mochňak,    <xmochn00@vutbr.cz>
 *                     Tibor Dudlák,   <xdudla00@vutbr.cz>
 *                     Dávid Prexta,   <xprext00@vutbr.cz>
 *                     Martin Krajňák, <xkrajn02@vutbr.cz>
 *                     Patrik Segedy,  <xseged00@vutbr.cz>
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * license.txt file in the root directory of this source tree.
 */

#ifndef ZVAL_H_
#define ZVAL_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "globals.h"

#define zval_set(v, T) _Generic((T),                        \
        unsigned int: zval_set_int,                         \
        int: zval_set_int,                                  \
        long unsigned int: zval_set_int,                    \
        double: zval_set_double,                            \
        char *: zval_set_string,                            \
        const char *: zval_set_string)(v, T)

#define ZVAL_GET_INT(x)     (zval_get_int((x)))
#define ZVAL_GET_DOUBLE(x)  (zval_get_double((x)))
#define ZVAL_GET_STRING(x)  (zval_get_string((x)))

#define ZVAL_SET_INT(x, v) zval_set(x, v)
#define ZVAL_SET_DOUBLE(x, v) zval_set(x, v)
#define ZVAL_SET_STRING(x, v) zval_set(x, v)

#define ZVAL_IS_INT(x)     ((x)->type == T_INT)
#define ZVAL_IS_DOUBLE(x)  ((x)->type == T_DOUBLE)
#define ZVAL_IS_STRING(x)  ((x)->type == T_STRING)
#define ZVAL_IS_DEFINED(x) ((x)->type != T_UNDEFINED)

#define ZVAL_INIT_INT(x, v)                                 \
    (x) = malloc(sizeof(zval_t));                           \
    ZVAL_SET_INT(x, v)                                      \

#define ZVAL_INIT_DOUBLE(x, v)                              \
    (x) = malloc(sizeof(zval_t));                           \
    ZVAL_SET_DOUBLE(x, v)                                   \

#define zval_init(x) zval_set_undefined((x))

typedef struct __zval_t zval_t;

enum __data_type {
    T_NOOP = 0,
    T_UNDEFINED,
    T_INT = 9,
    T_DOUBLE,
    T_STRING
};

struct __zval_t {
    enum __data_type type; // TODO: move to binary flags

    union {
        int iVal;
        double dVal;
        char *sVal;
    };
};

INLINED int zval_get_int(zval_t *val) {
    if (val->type != T_INT) {
        fprintf(stderr, "Trying to get int value of type %d", val->type);
    }

    return val->iVal;
}

INLINED double zval_get_double(zval_t *val) {
    if (val->type != T_DOUBLE) {
        fprintf(stderr, "Trying to get int value of type %d", val->type);
    }

    return val->dVal;
}

INLINED char *zval_get_string(zval_t *val) {
    if (val->type != T_STRING) {
        fprintf(stderr, "Trying to get int value of type %d", val->type);
    }

    return val->sVal;
}

INLINED result_t zval_set_undefined(zval_t *val) {

    val->type = T_UNDEFINED;
    val->sVal = NULL;
    return EOK;
}

INLINED result_t zval_set_int(zval_t *val, const int n) {

    val->type = T_INT;
    val->iVal = n;

    return EOK;
}

INLINED result_t zval_set_double(zval_t *val, const double n) {

    val->type = T_DOUBLE;
    val->dVal = n;

    return EOK;
}

INLINED result_t zval_set_string(zval_t *val, const char *str) {

    val->type = T_STRING;

    if ((val->sVal = malloc(sizeof(char) * (1 + strlen(str)))) == NULL) {
        return ESYS;
    }

    strcpy(val->sVal, str);

    return EOK;
}

INLINED bool zval_is_numeric(zval_t *val) {
    return ZVAL_IS_INT(val) || ZVAL_IS_DOUBLE(val);
}

INLINED void zval_dispose(zval_t *val) {

    if (val == NULL) {
        return;
    }

    if (ZVAL_IS_STRING(val)) {
        free(val->sVal);
    }
}

INLINED result_t zval_copy(zval_t *dest, zval_t *src) {

    if (dest == NULL || src == NULL) {
        return ESYS;
    }

    dest->dVal = src->dVal;
    dest->iVal = src->iVal;
    dest->type = src->type;

    if (ZVAL_IS_STRING(dest)) {
        return zval_set_string(dest, ZVAL_GET_STRING(src));
    }

    return EOK;
}



INLINED result_t zval_add(zval_t *dest, zval_t *a, zval_t *b) {

    if (a == NULL || b == NULL) return ESYS; // TODO: Fix Error code

    if (ZVAL_IS_DOUBLE(a) && ZVAL_IS_INT(b)) {
        zval_set(dest, (double) ZVAL_GET_DOUBLE(a) + ZVAL_GET_INT(b));
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_INT(a)) {
        zval_set(dest, (double) ZVAL_GET_INT(a) + ZVAL_GET_DOUBLE(b));
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_DOUBLE(a)) {
        zval_set(dest, ZVAL_GET_DOUBLE(a) + ZVAL_GET_DOUBLE(b));
    } else if (ZVAL_IS_INT(b) && ZVAL_IS_INT(a)) {
        zval_set(dest, ZVAL_GET_INT(a) + ZVAL_GET_INT(b));
    } else {
        return ESEM3; // TODO: Fix error code
    }

    return EOK;
}

#endif // ZVAL_H_
