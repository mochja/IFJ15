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

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdlib.h>
#include "klist.h"
#include "zval.h"

typedef struct __instruction_t instruction_t;

enum __instruction_type {
    I_LOAD,
    I_CALL,
    I_PUSH,
    I_POP,
    I_ADD
};

struct __instruction_t {
    enum __instruction_type type;

    zval_t *first;
    zval_t *second;
    zval_t *third;
};

static inline __attribute__ ((__unused__)) void destroy_instruction(instruction_t *i) {
    if (i == NULL) {
        return;
    }

    destroy_zval(i->first);
    destroy_zval(i->second);
    destroy_zval(i->third);

    free(i);
}

#define __instruction_free(x) destroy_instruction(kl_val(x))
KLIST_INIT(instruction_list, instruction_t*, __instruction_free)

/**
 * Instruction factories
 */

#define INSTR_T static inline __attribute__ ((__unused__)) instruction_t*

/**
 * Move first element from stack to [addr_offset]
 */
INSTR_T create_POP_instr(int addr_offset) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    i->type = I_POP;
    ZVAL_INIT_INT(i->first, addr_offset);

    return i;
}

#undef INSTR_T

#endif // INSTRUCTION_H
