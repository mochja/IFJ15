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
    I_STORE_zval, // store on top of call stack
    I_STORE,      // store to callstack offset
    I_LOAD,
    I_JMP,
    I_COUT_pop,
    I_COUT_offset,
    I_JMPE,

    I_GT,

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

INLINED void instruction_print(char *dest, instruction_t *i) {

    char buff[256];
    buff[0] = '\0';

    switch (i->type) {
        case I_NOOP:
            strcat(buff, "NOOP");
            break;
        case I_LABEL:
            strcat(buff, "LABEL");
            break;
        case I_POP:
            strcat(buff, "POP");
            break;
        case I_POP_N:
            strcat(buff, "POP_N");
            break;
        case I_POP_to:
            strcat(buff, "POP_to");
            break;
        case I_PUSH:
            strcat(buff, "PUSH");
            break;
        case I_PUSH_zval:
            strcat(buff, "PUSH ZVAL");
            break;
        case I_STORE_zval:
            strcat(buff, "STORE ZVAL");
            break;
        case I_STORE:
            strcat(buff, "STORE");
            break;
        case I_JMP:
            strcat(buff, "JMP");
            break;
        case I_COUT_pop:
            strcat(buff, "COUT POP");
            break;
        case I_COUT_offset:
            strcat(buff, "COUT OFFSET");
            break;
        case I_JMPE:
            strcat(buff, "JMPE");
            break;
        case I_GT:
            strcat(buff, "GT");
            break;
        case I_CALL:
            strcat(buff, "CALL");
            break;
        case I_RETURN:
            strcat(buff, "RETURN");
            break;
        case I_EXIT:
            strcat(buff, "EXIT");
            break;
        case I_ADD_zval:
            strcat(buff, "ADD ZVAL");
            break;
        case I_ADD_pop:
            strcat(buff, "ADD POP");
            break;
        case I_ADD_zval_pop:
            strcat(buff, "ZVAL_POP");
            break;
        case I_ADD_pop_zval:
            strcat(buff, "POP_ZVAL");
            break;
        case I_ADD_offset:
            strcat(buff, "ADD OFFSET");
            break;
        case I_LOAD:
            strcat(buff, "LOAD");
            break;
    }

    if (i->first != NULL && ZVAL_IS_DEFINED(i->first)) {
        if (ZVAL_IS_INT(i->first)) {
            sprintf(buff, "%s %d", buff, ZVAL_GET_INT(i->first));
        } else if (ZVAL_IS_DOUBLE(i->first)) {
            sprintf(buff, "%s %f", buff, ZVAL_GET_DOUBLE(i->first));
        } else if (ZVAL_IS_STRING(i->first)) {
            sprintf(buff, "%s %s", buff, ZVAL_GET_STRING(i->first));
        } else {
            strcat(buff, " [?!?!?!]");
        }
    } else {
        strcat(buff, " [NULL]");
    }

    if (i->second != NULL && ZVAL_IS_DEFINED(i->second)) {
        if (ZVAL_IS_INT(i->second)) {
            sprintf(buff, "%s %d", buff, ZVAL_GET_INT(i->second));
        } else if (ZVAL_IS_DOUBLE(i->second)) {
            sprintf(buff, "%s %f", buff, ZVAL_GET_DOUBLE(i->second));
        } else if (ZVAL_IS_STRING(i->second)) {
            sprintf(buff, "%s %s", buff, ZVAL_GET_STRING(i->second));
        } else {
            strcat(buff, " [?!?!?!]");
        }
    } else {
        strcat(buff, " [NULL]");
    }

    if (i->third != NULL && ZVAL_IS_DEFINED(i->third)) {
        if (ZVAL_IS_INT(i->third)) {
            sprintf(buff, "%s %d", buff, ZVAL_GET_INT(i->third));
        } else if (ZVAL_IS_DOUBLE(i->third)) {
            sprintf(buff, "%s %f", buff, ZVAL_GET_DOUBLE(i->third));
        } else if (ZVAL_IS_STRING(i->third)) {
            sprintf(buff, "%s %s", buff, ZVAL_GET_STRING(i->third));
        } else {
            strcat(buff, " [?!?!?!]");
        }
    } else {
        strcat(buff, " [NULL]");
    }

    buff[256] = '\0';

    strcpy(dest, buff);
}

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

#define __instruction_free(x) do { instruction_dispose(kl_val(x)); free(kl_val(x)); } while (0);
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
    i->second = NULL;
    i->third = NULL;

    return i;
}




INSTR_T create_LABEL_instr(int key) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    i->type = I_LABEL;
    ZVAL_INIT_INT(i->first, key);
    i->second = NULL;
    i->third = NULL;

    return i;
}



INLINED result_t create_RETURN_instr(instruction_t *i) {

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
    i->second = NULL;
    i->third = NULL;

    return i;
}




INLINED result_t create_POP_N_instr(instruction_t *i, const int n) {

    i->type = I_POP_N;
    ZVAL_INIT_INT(i->first, n);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INSTR_T create_PUSH_int_instr(const int store_offset) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    i->type = I_PUSH;
    ZVAL_INIT_INT(i->first, store_offset);
    i->second = NULL;
    i->third = NULL;

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



INLINED result_t create_STORE_zval_instr(instruction_t *i, zval_t *val) {

    i->type = I_STORE_zval;

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



INLINED result_t create_STORE_instr(instruction_t *i, const int offset) {

    i->type = I_STORE;

    i->first = malloc(sizeof(zval_t));
    zval_set(i->first, offset);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_CALL_instr(instruction_t *i, const int label, const int param_count) {

    i->type = I_CALL;

    ZVAL_INIT_INT(i->first, label);
    ZVAL_INIT_INT(i->second, param_count);
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
    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return i;
}



INLINED result_t create_COUT_offset_instr(instruction_t *i, const int offset) {

    i->type = I_COUT_offset;
    ZVAL_INIT_INT(i->first, offset);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_JMPE_instr(instruction_t *i, const int label_id, zval_t *equals_to) {

    i->type = I_JMPE;

    ZVAL_INIT_INT(i->first, label_id);
    i->second = malloc(sizeof(zval_t));
    zval_copy(i->second, equals_to);
    i->third = NULL;

    return EOK;
}



INLINED result_t create_LOAD_instr(instruction_t *i, zval_t *a) {

    i->type = I_LOAD;

    i->first = malloc(sizeof(zval_t));
    zval_copy(i->first, a); // offset
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_GT_instr(instruction_t *i, zval_t *a, zval_t *b) {

    i->type = I_GT;

    i->first = malloc(sizeof(zval_t));
    i->second = malloc(sizeof(zval_t));
    zval_copy(i->first, a);
    zval_copy(i->second, b);

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
