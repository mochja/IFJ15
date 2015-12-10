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
#include "expr.h"
#include "kvec.h"

typedef struct __instruction_t instruction_t;

enum __instruction_type {
    I_NOOP,
    I_LABEL,
    I_POP,
    I_POP_N,
    I_POP_to,
    I_PUSH,
    I_PUSH_zval,
    I_JMP,
    I_COUT_pop,
    I_COUT_offset,

    I_CALL,
    I_RETURN,
    I_EXIT,

    I_ADD_zval,
    I_ADD_pop,
    I_ADD_zval_pop,
    I_ADD_pop_zval,

    I_ADD_offset,
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

    if (i == NULL) {
        return;
    }

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
result_t append_instr_from_expr(klist_t(instruction_list) *dest, klist_t(expr_stack) *expression);

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



INSTR_T create_RETURN_instr(instruction_t *i) {

    i->type = I_RETURN;
    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}




INSTR_T create_JMP_instr(int label_key) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    i->type = I_JMP;
    ZVAL_INIT_INT(i->first, label_key);

    return i;
}




INLINED result_t create_POP_N_instr(instruction_t *i, const int n) {

    i->type = I_POP_N;
    ZVAL_INIT_INT(i->first, n);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_POP_to_instr(instruction_t *i, const int offset) {

    i->type = I_POP_to;
    ZVAL_INIT_INT(i->first, offset);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INSTR_T create_PUSH_int_instr(const int store_offset) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    i->type = I_PUSH;
    ZVAL_INIT_INT(i->first, store_offset);

    return i;
}



INLINED result_t create_PUSH_zval_instr(instruction_t *i, zval_t *val) {

    i->type = I_PUSH_zval;

    i->first = malloc(sizeof(zval_t));

    if (val != NULL) {
        zval_copy(i->first, val);
    } else {
        zval_init(i->first);
    }

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_CALL_instr(instruction_t *i, const int label) {

    i->type = I_CALL;

    ZVAL_INIT_INT(i->first, label);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_EXIT_instr(instruction_t *i) {

    i->type = I_EXIT;

    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}




INSTR_T create_COUT_pop_instr() {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    i->type = I_COUT_pop;

    return i;
}



INLINED result_t create_COUT_offset_instr(instruction_t *i, const int offset) {

    i->type = I_COUT_offset;
    ZVAL_INIT_INT(i->first, offset);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * ADD zval
 * param can be int or double
 */
INLINED result_t create_ADD_zval_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_ADD_zval;

    if (!zval_is_numeric(a) || !zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }

    if ((i->second = malloc(sizeof(zval_t))) == NULL) {
        return ESYS;
    }

    zval_copy(i->first, a);
    zval_copy(i->second, b);

    i->third = NULL;

    return EOK;
}




/**
 * ADD
 * use first as constant and pop the 2nd param
 */
INLINED result_t create_ADD_zval_pop_instr(instruction_t *i, zval_t *a) {

    i->type = I_ADD_zval_pop;

    if (!zval_is_numeric(a)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        free(i);
        return ESYS;
    }

    zval_copy(i->first, a);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * ADD
 * use first as constant and pop the 2nd param
 */
INLINED result_t create_ADD_pop_zval_instr(instruction_t *i, zval_t *b) {

    i->type = I_ADD_pop_zval;

    if (!zval_is_numeric(b)) {
        return ESEM4; // TODO: proper error code
    }

    if ((i->first = malloc(sizeof(zval_t))) == NULL) {
        free(i);
        return ESYS;
    }

    zval_copy(i->first, b);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



/**
 * ADD
 * use first as constant and pop the 2nd param
 */
INLINED result_t create_ADD_pop_instr(instruction_t *i) {

    i->type = I_ADD_pop;

    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_ADD_offset_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_ADD_offset;

    zval_copy(i->first, a);
    zval_copy(i->second, b);
    i->third = NULL;

    return EOK;
}
#endif // INSTRUCTION_H
