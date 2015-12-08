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
#include "expression.h"
#include "kvec.h"

typedef struct __instruction_t instruction_t;

enum __instruction_type {
    I_NOOP,
    I_LABEL,
    I_POP,
    I_PUSH,
    I_JMP,
    I_COUT_pop,

    I_ADDI_int,
    I_ADD_pop,
    I_ADDI_pop_int,
    I_ADDI_int_pop,
    I_SUBI_int,
    I_SUB_pop,
    I_SUBI_pop_int,
    I_SUBI_int_pop,
    I_MULI_int,
    I_MUL_pop,
    I_MULI_pop_int,
    I_MULI_int_pop,
    I_DIVI_int,
    I_DIV_pop,
    I_DIVI_pop_int,
    I_DIVI_int_pop,
    I_ADDD_double,
    I_ADDD_pop_double,
    I_ADDD_double_pop,
    I_SUBD_double,
    I_SUBD_pop_double,
    I_SUBD_double_pop,
    I_MULD_double,
    I_MULD_pop_double,
    I_MULD_double_pop,
    I_DIVD_double,
    I_DIVD_pop_double,
    I_DIVD_double_pop
};

struct __instruction_t {
    enum __instruction_type type;

    zval_t *first;
    zval_t *second;
    zval_t *third;
};

INLINED void instruction_init(instruction_t *i) {

    i->type = I_NOOP;

    i->first = NULL;
    i->second = NULL;
    i->third = NULL;
}

INLINED void instruction_dispose(instruction_t *i) {

    zval_dispose(i->first); free(i->first);
    zval_dispose(i->second); free(i->second);
    zval_dispose(i->third); free(i->third);

    i->first = NULL;
    i->second = NULL;
    i->third = NULL;
}

#define __copy_if_src_exists(name, dest, src)        \
    if ((src)->name) {                               \
        (dest)->name = malloc(sizeof(zval_t));       \
        zval_copy((dest)->name, (src)->name);        \
    } else (dest)->name = 0;

INLINED void instruction_copy(instruction_t *dest, instruction_t *src) {
    if (dest == NULL || src == NULL) {
        if (dest != NULL) {
            dest->type = I_NOOP;
        }
        return;
    }

    dest->type = src->type;

    __copy_if_src_exists(first, dest, src);
    __copy_if_src_exists(second, dest, src);
    __copy_if_src_exists(third, dest, src);
}

#define __instruction_free(x) instruction_dispose(kl_val(x)); free(kl_val(x));
KLIST_INIT(instruction_list, instruction_t*, __instruction_free)


/**
 * Create instructions from given expression
 */
klist_t(instruction_list) *create_instructions_from_expression(klist_t(expr_stack) *expression);

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




INSTR_T create_PUSH_int_instr(const int store_offset) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    i->type = I_PUSH;
    ZVAL_INIT_INT(i->first, store_offset);

    return i;
}




INSTR_T create_COUT_pop_instr() {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    i->type = I_COUT_pop;

    return i;
}


#endif // INSTRUCTION_H
