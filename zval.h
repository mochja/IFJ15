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

#define ZVAL_GET_INT(x)     ((x)->iVal)
#define ZVAL_GET_DOUBLE(x)  ((x)->dVal)
#define ZVAL_GET_STRING(x)  ((x)->sVal)

#define ZVAL_SET_INT(x, v) (x)->type = T_INT;                \
    (x)->iVal = v

#define ZVAL_SET_DOUBLE(x, v) (x)->type = T_DOUBLE;         \
    (x)->dVal = v

#define ZVAL_SET_STRING(x, v) (x)->type = T_STRING;         \
    (x)->sVal = malloc((strlen(x) + 1) * sizeof(char))      \
    strcpy((x)->sVal, x);

#define ZVAL_INIT_INT(x, v)                                 \
    (x) = malloc(sizeof(zval_t));                           \
    ZVAL_SET_INT(x, v)                                      \

#define ZVAL_INIT_DOUBLE(x, v)                              \
    (x) = malloc(sizeof(zval_t));                           \
    ZVAL_SET_DOUBLE(x, v)                                   \

#define ZVAL_IS_INT(x) ((x)->type == T_INT)
#define ZVAL_IS_DOUBLE(x) ((x)->type == T_DOUBLE)
#define ZVAL_IS_STRING(x) ((x)->type == T_STRING)

typedef struct __zval_t zval_t;

enum __data_type {
    T_INT,
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

static inline __attribute__ ((__unused__)) void destroy_zval(zval_t *val) {
    if (val == NULL) {
        return;
    }

    if (ZVAL_IS_STRING(val)) {
        free(val->sVal);
    }

    free(val);
}

#endif // ZVAL_H_
