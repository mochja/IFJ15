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
#include <string.h>
#include "klist.h"
#include "zval.h"

typedef struct __instruction_t instruction_t;

enum __instruction_type {
    I_NOOP,
    I_LABEL,
    I_POP,
    I_JMP,
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

#define __copy_if_src_exists(name, dest, src)        \
    if ((src)->name) {                               \
        (dest)->name = malloc(sizeof(zval_t));       \
        copy_zval((dest)->name, (src)->name);        \
    } else (dest)->name = 0;

static inline __attribute__ ((__unused__)) void copy_instruction(instruction_t *dest, instruction_t *src) {
    if (dest == NULL || src == NULL) {
        return;
    }

    memcpy(dest, src, sizeof(instruction_t));

    __copy_if_src_exists(first, dest, src);
    __copy_if_src_exists(second, dest, src);
    __copy_if_src_exists(third, dest, src);
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

INSTR_T create_LABEL_instr(int key) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    i->type = I_LABEL;
    ZVAL_INIT_INT(i->first, key);

    return i;
}

INSTR_T create_JMP_instr(int label_key) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    i->type = I_JMP;
    ZVAL_INIT_INT(i->first, label_key);

    return i;
}

#undef INSTR_T

#endif // INSTRUCTION_H
