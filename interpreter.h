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


typedef struct __stack_item_t stack_item_t;
typedef struct __zvar_t zvar_t;

struct __zvar_t {
    char *name;
    int dataType;

    union {
        int iVal;
        double dVal;
        char *sVal;
    };
};

enum instruction_type {
    I_LOAD,
    I_CALL,
    I_PUSH,
    I_POP,
    I_ADD
};

enum data_type {
    T_INT,
    T_DOUBLE,
    T_STRING
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
