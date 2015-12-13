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

#define ZVAL_GET_INT(x)         (zval_get_int((x)))
#define ZVAL_GET_DOUBLE(x)      (zval_get_double((x)))
#define ZVAL_GET_STRING(x)      (zval_get_string((x)))

#define ZVAL_SET_INT(x, v)      zval_set(x, v)
#define ZVAL_SET_DOUBLE(x, v)   zval_set(x, v)
#define ZVAL_SET_STRING(x, v)   zval_set(x, v)

#define ZVAL_IS_INT(x)          (((x)->type >> 1) & 1)
#define ZVAL_IS_DOUBLE(x)       (((x)->type >> 2) & 1)
#define ZVAL_IS_STRING(x)       (((x)->type >> 3) & 1)
#define ZVAL_IS_DEFINED(x)      (!(((x)->type >> 0) & 1))

#define ZVAL_INIT_INT(x, v)                                 \
    (x) = malloc(sizeof(zval_t));                           \
    ZVAL_SET_INT(x, v)                                      \

#define ZVAL_INIT_DOUBLE(x, v)                              \
    (x) = malloc(sizeof(zval_t));                           \
    ZVAL_SET_DOUBLE(x, v)                                   \

typedef struct __zval_t zval_t;

#define T_UNDEFINED     0x01U
#define T_INT           0x02U
#define T_DOUBLE        0x04U
#define T_STRING        0x08U

struct __zval_t {
    unsigned char type; // TODO: move to binary flags

    union {
        int iVal;
        double dVal;
        char *sVal;
    };
};


INLINED result_t zval_init(zval_t *val) {

    val->type = T_UNDEFINED;
    val->sVal = NULL;

    return EOK;
}



INLINED int zval_get_int(zval_t *val) {
    if (val->type != T_INT) {
        fprintf(stderr, "Trying to get int value of type %d\n", val->type);
    }

    return val->iVal;
}



INLINED double zval_get_double(zval_t *val) {
    if (val->type != T_DOUBLE) {
        fprintf(stderr, "Trying to get double value of type %d\n", val->type);
    }

    return val->dVal;
}



INLINED char *zval_get_string(zval_t *val) {
    if (val->type != T_STRING && val->type != 0x09) {
        fprintf(stderr, "Trying to get string value of type %d\n", val->type);
    }

    return val->sVal;
}



INLINED result_t zval_set_undefined(zval_t *val) {

    val->type |= T_UNDEFINED;

    if (ZVAL_IS_DEFINED(val) && ZVAL_IS_STRING(val)) {
        free(val->sVal);
    }

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

    dest->type = src->type;

    if (ZVAL_IS_STRING(src)) {
        if (ZVAL_IS_STRING(dest)) {
            zval_dispose(dest);
        }
        return zval_set_string(dest, ZVAL_GET_STRING(src));
    } else if (!ZVAL_IS_DOUBLE(src)) {
        dest->iVal = src->iVal;
    } else {
        dest->dVal = src->dVal;
    }

    return EOK;
}



INLINED result_t zval_cast(zval_t *dest, zval_t *src) {

    if (dest == NULL || src == NULL) {
        return ESYS;
    }

    if (dest->type == T_UNDEFINED) {
        return zval_copy(dest, src);
    }

    if (ZVAL_IS_STRING(dest) && ZVAL_IS_STRING(src)) {
        zval_dispose(dest);
        return zval_set_string(dest, ZVAL_GET_STRING(src));
    } else if (!ZVAL_IS_DOUBLE(dest)) {
        if (ZVAL_IS_INT(src)) {
            return zval_set(dest, zval_get_int(src));
        } else if (ZVAL_IS_DOUBLE(src)) {
            return zval_set(dest, (int) zval_get_double(src));
        } else return ESEM2;
    } else {
        if (ZVAL_IS_INT(src)) {
            return zval_set(dest, (double) zval_get_int(src));
        } else if (ZVAL_IS_DOUBLE(src)) {
            return zval_set(dest, zval_get_double(src));
        } else if (ZVAL_IS_STRING(src)) {
            return zval_set_string(dest, zval_get_string(src));
        } else return ESEM2;
    }
}



INLINED result_t zval_add(zval_t *dest, zval_t *a, zval_t *b) {

    if (a == NULL || b == NULL || !ZVAL_IS_DEFINED(a) || !ZVAL_IS_DEFINED(b)) return ESYS; // TODO: Fix Error code

    if (ZVAL_IS_DOUBLE(a) && ZVAL_IS_INT(b)) {
        zval_set(dest, ZVAL_GET_DOUBLE(a) + (double) ZVAL_GET_INT(b));
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_INT(a)) {
        zval_set(dest, (double) ZVAL_GET_INT(a) + ZVAL_GET_DOUBLE(b));
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_DOUBLE(a)) {
        zval_set(dest, ZVAL_GET_DOUBLE(a) + ZVAL_GET_DOUBLE(b));
    } else if (ZVAL_IS_INT(b) && ZVAL_IS_INT(a)) {
        zval_set(dest, ZVAL_GET_INT(a) + ZVAL_GET_INT(b));
    } else {
        return ESEM2; // TODO: Fix error code
    }

    return EOK;
}



INLINED result_t zval_sub(zval_t *dest, zval_t *a, zval_t *b) {

    if (a == NULL || b == NULL || !ZVAL_IS_DEFINED(a) || !ZVAL_IS_DEFINED(b)) return ESYS; // TODO: Fix Error code

    if (ZVAL_IS_DOUBLE(a) && ZVAL_IS_INT(b)) {
        zval_set(dest, ZVAL_GET_DOUBLE(a) - (double) ZVAL_GET_INT(b));
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_INT(a)) {
        zval_set(dest, (double) ZVAL_GET_INT(a) - ZVAL_GET_DOUBLE(b));
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_DOUBLE(a)) {
        zval_set(dest, ZVAL_GET_DOUBLE(a) - ZVAL_GET_DOUBLE(b));
    } else if (ZVAL_IS_INT(b) && ZVAL_IS_INT(a)) {
        zval_set(dest, ZVAL_GET_INT(a) - ZVAL_GET_INT(b));
    } else {
        return ESEM2; // TODO: Fix error code
    }

    return EOK;
}



INLINED result_t zval_mul(zval_t *dest, zval_t *a, zval_t *b) {

    if (a == NULL || b == NULL || !ZVAL_IS_DEFINED(a) || !ZVAL_IS_DEFINED(b)) return ESYS; // TODO: Fix Error code

    if (ZVAL_IS_DOUBLE(a) && ZVAL_IS_INT(b)) {
        zval_set(dest, ZVAL_GET_DOUBLE(a) * (double) ZVAL_GET_INT(b));
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_INT(a)) {
        zval_set(dest, (double) ZVAL_GET_INT(a) * ZVAL_GET_DOUBLE(b));
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_DOUBLE(a)) {
        zval_set(dest, ZVAL_GET_DOUBLE(a) * ZVAL_GET_DOUBLE(b));
    } else if (ZVAL_IS_INT(b) && ZVAL_IS_INT(a)) {
        zval_set(dest, ZVAL_GET_INT(a) * ZVAL_GET_INT(b));
    } else {
        return ESEM2; // TODO: Fix error code
    }

    return EOK;
}



INLINED result_t zval_div(zval_t *dest, zval_t *a, zval_t *b) {
    if (a == NULL || b == NULL || !ZVAL_IS_DEFINED(a) || !ZVAL_IS_DEFINED(b)) return ESYS; // TODO: Fix Error code if (a == NULL || b == NULL) return ESYS; // TODO: Fix Error code

    if (((ZVAL_IS_INT(b)) && (ZVAL_GET_INT(b) == 0)) || ((ZVAL_IS_DOUBLE(b)) && (ZVAL_GET_DOUBLE(b) == 0.0))) {
        return ERUN2;
    }

    if (ZVAL_IS_DOUBLE(a) && ZVAL_IS_INT(b)) {
        zval_set(dest, ZVAL_GET_DOUBLE(a) / (double) ZVAL_GET_INT(b));
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_INT(a)) {
        zval_set(dest, (double) ZVAL_GET_INT(a) / ZVAL_GET_DOUBLE(b));
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_DOUBLE(a)) {
        zval_set(dest, ZVAL_GET_DOUBLE(a) / ZVAL_GET_DOUBLE(b));
    } else if (ZVAL_IS_INT(b) && ZVAL_IS_INT(a)) {
        zval_set(dest, (double) ZVAL_GET_INT(a) / ZVAL_GET_INT(b));
    } else {
        return ESEM2; // TODO: Fix error code
    }

    return EOK;
}



/**
 * set dest value to 1 when a and b are equal, 0 otherwise
 */
INLINED result_t zval_eq(zval_t *dest, zval_t *a, zval_t *b) {
    if (a == NULL || b == NULL || !ZVAL_IS_DEFINED(a) || !ZVAL_IS_DEFINED(b)) return ESYS; // TODO: Fix Error code

    if (ZVAL_IS_DOUBLE(a) && ZVAL_IS_INT(b)) {
        int res = (ZVAL_GET_DOUBLE(a) == (double) ZVAL_GET_INT(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_INT(a)) {
        int res = ((double) ZVAL_GET_INT(a) == ZVAL_GET_DOUBLE(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_DOUBLE(a)) {
        int  res = (ZVAL_GET_DOUBLE(a) == ZVAL_GET_DOUBLE(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_INT(b) && ZVAL_IS_INT(a)) {
        int res = (ZVAL_GET_INT(a) == ZVAL_GET_INT(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_STRING(b) && ZVAL_IS_STRING(a)) {
        int res = (!strcmp(zval_get_string(a), zval_get_string(b))) ? 1 : 0;
        zval_set(dest, res);
    } else {
        return ESEM2; // TODO: Fix error code
    }

    return EOK;
}


INLINED result_t zval_nq(zval_t *dest, zval_t *a, zval_t *b) {
    if (a == NULL || b == NULL || !ZVAL_IS_DEFINED(a) || !ZVAL_IS_DEFINED(b)) return ESYS; // TODO: Fix Error code

    if (ZVAL_IS_DOUBLE(a) && ZVAL_IS_INT(b)) {
        int res = (ZVAL_GET_DOUBLE(a) != (double) ZVAL_GET_INT(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_INT(a)) {
        int res = ((double) ZVAL_GET_INT(a) != ZVAL_GET_DOUBLE(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_DOUBLE(a)) {
        int  res = (ZVAL_GET_DOUBLE(a) != ZVAL_GET_DOUBLE(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_INT(b) && ZVAL_IS_INT(a)) {
        int res = (ZVAL_GET_INT(a) != ZVAL_GET_INT(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_STRING(b) && ZVAL_IS_STRING(a)) {
        int res = (strcmp(zval_get_string(a), zval_get_string(b))) ? 1 : 0;
        zval_set(dest, res);
    } else {
        return ESEM2; // TODO: Fix error code
    }

    return EOK;
}



/**
 * set dest value to 1 when a is greater than b, 0 otherwise
 */
INLINED result_t zval_gt(zval_t *dest, zval_t *a, zval_t *b) {
    if (a == NULL || b == NULL || !ZVAL_IS_DEFINED(a) || !ZVAL_IS_DEFINED(b)) return ESYS; // TODO: Fix Error code

    if (ZVAL_IS_DOUBLE(a) && ZVAL_IS_INT(b)) {
        int res = (ZVAL_GET_DOUBLE(a) > (double) ZVAL_GET_INT(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_INT(a)) {
        int res = ((double) ZVAL_GET_INT(a) > ZVAL_GET_DOUBLE(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_DOUBLE(a)) {
        int  res = (ZVAL_GET_DOUBLE(a) > ZVAL_GET_DOUBLE(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_INT(b) && ZVAL_IS_INT(a)) {
        int res = (ZVAL_GET_INT(a) > ZVAL_GET_INT(b)) ? 1 : 0;
        zval_set(dest, res);
    } else {
        return ESEM2; // TODO: Fix error code
    }

    return EOK;
}


INLINED result_t zval_lt(zval_t *dest, zval_t *a, zval_t *b) {
    if (a == NULL || b == NULL || !ZVAL_IS_DEFINED(a) || !ZVAL_IS_DEFINED(b)) return ESYS; // TODO: Fix Error code

    if (ZVAL_IS_DOUBLE(a) && ZVAL_IS_INT(b)) {
        int res = (ZVAL_GET_DOUBLE(a) < (double) ZVAL_GET_INT(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_INT(a)) {
        int res = ((double) ZVAL_GET_INT(a) < ZVAL_GET_DOUBLE(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_DOUBLE(a)) {
        int  res = (ZVAL_GET_DOUBLE(a) < ZVAL_GET_DOUBLE(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_INT(b) && ZVAL_IS_INT(a)) {
        int res = (ZVAL_GET_INT(a) < ZVAL_GET_INT(b)) ? 1 : 0;
        zval_set(dest, res);
    } else {
        return ESEM2; // TODO: Fix error code
    }

    return EOK;
}



/**
 * set dest value to 1 when a is less or equals b, 0 otherwise
 */
INLINED result_t zval_le(zval_t *dest, zval_t *a, zval_t *b) {
    if (a == NULL || b == NULL || !ZVAL_IS_DEFINED(a) || !ZVAL_IS_DEFINED(b)) return ESYS; // TODO: Fix Error code

    if (ZVAL_IS_DOUBLE(a) && ZVAL_IS_INT(b)) {
        int res = (ZVAL_GET_DOUBLE(a) <= (double) ZVAL_GET_INT(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_INT(a)) {
        int res = ((double) ZVAL_GET_INT(a) <= ZVAL_GET_DOUBLE(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_DOUBLE(a)) {
        int  res = (ZVAL_GET_DOUBLE(a) <= ZVAL_GET_DOUBLE(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_INT(b) && ZVAL_IS_INT(a)) {
        int res = (ZVAL_GET_INT(a) <= ZVAL_GET_INT(b)) ? 1 : 0;
        zval_set(dest, res);
    } else {
        return ESEM2; // TODO: Fix error code
    }

    return EOK;
}



/**
 * set dest value to 1 when a is greater or equals b, 0 otherwise
 */
INLINED result_t zval_ge(zval_t *dest, zval_t *a, zval_t *b) {
    if (a == NULL || b == NULL || !ZVAL_IS_DEFINED(a) || !ZVAL_IS_DEFINED(b)) return ESYS; // TODO: Fix Error code

    if (ZVAL_IS_DOUBLE(a) && ZVAL_IS_INT(b)) {
        int res = (ZVAL_GET_DOUBLE(a) >= (double) ZVAL_GET_INT(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_INT(a)) {
        int res = ((double) ZVAL_GET_INT(a) >= ZVAL_GET_DOUBLE(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_DOUBLE(b) && ZVAL_IS_DOUBLE(a)) {
        int  res = (ZVAL_GET_DOUBLE(a) >= ZVAL_GET_DOUBLE(b)) ? 1 : 0;
        zval_set(dest, res);
    } else if (ZVAL_IS_INT(b) && ZVAL_IS_INT(a)) {
        int res = (ZVAL_GET_INT(a) >= ZVAL_GET_INT(b)) ? 1 : 0;
        zval_set(dest, res);
    } else {
        return ESEM2; // TODO: Fix error code
    }

    return EOK;
}



INLINED void zval_print(zval_t *val) {

    if (ZVAL_IS_DEFINED(val)) {
        if (ZVAL_IS_INT(val)) {
            printf("%d", zval_get_int(val));
        } else if (ZVAL_IS_DOUBLE(val)) {
            printf("%g", zval_get_double(val));
        } else if (ZVAL_IS_STRING(val)) {
            printf("%s", zval_get_string(val));
        }
    } else {
        printf("[null]\n");
    }
}

#endif // ZVAL_H_
