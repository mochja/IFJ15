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
 * LICENSE file in the root directory of this source tree.
 */

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include "klist.h"

#define ZVAL_GET_INT(x) (x)->iVal
#define ZVAL_GET_DOUBLE(x) (x)->dVal
#define ZVAL_GET_STRING(x) (x)->sVal

#define ZVAL_SET_INT(x, v) (x)->type = T_INT; \
    (x)->iVal = v

#define ZVAL_SET_DOUBLE(x, v) (x)->type = T_DOUBLE; \
    (x)->dVal = v

#define ZVAL_SET_STRING(x, v) (x)->type = T_STRING; \
    (x)->sVal = malloc(strlen(x) + 1 * sizeof(char)) \
    memset((x)->sVal, x, strlen(x) + 1);

#define ZVAL_IS_INT(x) ((x)->type == T_INT)
#define ZVAL_IS_DOUBLE(x) ((x)->type == T_DOUBLE)
#define ZVAL_IS_STRING(x) ((x)->type == T_STRING)

typedef struct __stack_item_t stack_item_t;
typedef struct __zvar_t zvar_t;

enum instruction_type {
    I_LOAD,
    I_CALL,
    I_PUSH,
    I_POP,
    I_ADD
};

enum __data_type {
    T_INT,
    T_DOUBLE,
    T_STRING
};

struct __zvar_t {
    char *name;
    enum __data_type type;

    union {
        int iVal;
        double dVal;
        char *sVal;
    };
};

struct __stack_item_t {
    enum instruction_type type;

    zvar_t *first;
    zvar_t *second;
    zvar_t *third;
};

#define __item_free(x)
KLIST_INIT(stack_list, stack_item_t*, __item_free)

void interpret(klist_t(stack_list) *stack);

#endif // INTERPRETER_H_
