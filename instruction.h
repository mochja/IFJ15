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
    I_COUT_zval,
    I_CIN_offset,
    I_JMPE,

    I_CALL,
    I_CALLN,
    I_RETURN,
    I_EXIT,

    I_ADD_pop,
    I_ADD_pop_zval,
    I_ADD_pop_offset,
    I_ADD_offset,
    I_ADD_offset_zval,
    I_ADD_offset_pop,
    I_ADD_zval,
    I_ADD_zval_pop,
    I_ADD_zval_offset,
    I_SUB_pop,
    I_SUB_pop_zval,
    I_SUB_pop_offset,
    I_SUB_offset,
    I_SUB_offset_zval,
    I_SUB_offset_pop,
    I_SUB_zval,
    I_SUB_zval_pop,
    I_SUB_zval_offset,
    I_MUL_pop,
    I_MUL_pop_zval,
    I_MUL_pop_offset,
    I_MUL_offset,
    I_MUL_offset_zval,
    I_MUL_offset_pop,
    I_MUL_zval,
    I_MUL_zval_pop,
    I_MUL_zval_offset,
    I_DIV_pop,
    I_DIV_pop_zval,
    I_DIV_pop_offset,
    I_DIV_offset,
    I_DIV_offset_zval,
    I_DIV_offset_pop,
    I_DIV_zval,
    I_DIV_zval_pop,
    I_DIV_zval_offset,
    I_LT_pop,
    I_LT_pop_zval,
    I_LT_pop_offset,
    I_LT_offset,
    I_LT_offset_zval,
    I_LT_offset_pop,
    I_LT_zval,
    I_LT_zval_pop,
    I_LT_zval_offset,
    I_GT_pop,
    I_GT_pop_zval,
    I_GT_pop_offset,
    I_GT_offset,
    I_GT_offset_zval,
    I_GT_offset_pop,
    I_GT_zval,
    I_GT_zval_pop,
    I_GT_zval_offset,
    I_LE_pop,
    I_LE_pop_zval,
    I_LE_pop_offset,
    I_LE_offset,
    I_LE_offset_zval,
    I_LE_offset_pop,
    I_LE_zval,
    I_LE_zval_pop,
    I_LE_zval_offset,
    I_GE_pop,
    I_GE_pop_zval,
    I_GE_pop_offset,
    I_GE_offset,
    I_GE_offset_zval,
    I_GE_offset_pop,
    I_GE_zval,
    I_GE_zval_pop,
    I_GE_zval_offset,
    I_EQ_pop,
    I_EQ_pop_zval,
    I_EQ_pop_offset,
    I_EQ_offset,
    I_EQ_offset_zval,
    I_EQ_offset_pop,
    I_EQ_zval,
    I_EQ_zval_pop,
    I_EQ_zval_offset,
    I_NQ_pop,
    I_NQ_pop_zval,
    I_NQ_pop_offset,
    I_NQ_offset,
    I_NQ_offset_zval,
    I_NQ_offset_pop,
    I_NQ_zval,
    I_NQ_zval_pop,
    I_NQ_zval_offset
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
        default:
            sprintf(buff, "[%d]", i->type);
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

    buff[255] = '\0';

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
        zval_init((dest)->name);                     \
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

#define __instruction_free(x) do { instruction_dispose(&kl_val(x)); } while (0);
KLIST_INIT(instruction_list, instruction_t, __instruction_free)


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
INLINED result_t create_POP_instr(instruction_t *i, int addr_offset) {

    i->type = I_POP;
    ZVAL_INIT_INT(i->first, addr_offset);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}




INLINED result_t create_LABEL_instr(instruction_t *i, int key) {

    i->type = I_LABEL;
    ZVAL_INIT_INT(i->first, key);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_RETURN_instr(instruction_t *i) {

    i->type = I_RETURN;
    i->first = NULL;
    i->second = NULL;
    i->third = NULL;

    return EOK;
}




INLINED result_t create_JMP_instr(instruction_t *i, int label_key) {

    i->type = I_JMP;
    ZVAL_INIT_INT(i->first, label_key);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}




INLINED result_t create_POP_N_instr(instruction_t *i, const int n) {

    i->type = I_POP_N;
    ZVAL_INIT_INT(i->first, n);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_PUSH_int_instr(instruction_t *i, const int store_offset) {

    i->type = I_PUSH;
    ZVAL_INIT_INT(i->first, store_offset);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_PUSH_zval_instr(instruction_t *i, zval_t *val) {

    i->type = I_PUSH_zval;

    i->first = malloc(sizeof(zval_t));
    zval_init(i->first);

    if (val != NULL) {
        zval_copy(i->first, val);
    }

    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_CALLN_instr(instruction_t *i, const char *name) {

    i->type = I_CALLN;
    i->first = malloc(sizeof(zval_t));
    zval_set(i->first, name);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}


INLINED result_t create_STORE_zval_instr(instruction_t *i, zval_t *val) {

    i->type = I_STORE_zval;

    i->first = malloc(sizeof(zval_t));
    zval_init(i->first);

    if (val != NULL) {
        zval_copy(i->first, val);
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



INLINED result_t create_COUT_offset_instr(instruction_t *i, const int offset) {

    i->type = I_COUT_offset;
    ZVAL_INIT_INT(i->first, offset);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_COUT_zval_instr(instruction_t *i, zval_t *val) {

    i->type = I_COUT_zval;
    i->first = malloc(sizeof(zval_t));
    zval_init(i->first);
    zval_copy(i->first, val);
    i->second = NULL;
    i->third = NULL;

    return EOK;
}



INLINED result_t create_JMPE_instr(instruction_t *i, const int label_id, zval_t *equals_to) {

    i->type = I_JMPE;

    ZVAL_INIT_INT(i->first, label_id);
    i->second = malloc(sizeof(zval_t));
    zval_init(i->second);
    zval_copy(i->second, equals_to);
    i->third = NULL;

    return EOK;
}



INLINED result_t create_LOAD_instr(instruction_t *i, zval_t *a) {

    i->type = I_LOAD;

    i->first = malloc(sizeof(zval_t));
    zval_init(i->first);
    zval_copy(i->first, a); // offset
    i->second = NULL;
    i->third = NULL;

    return EOK;
}


INLINED result_t create_CIN_offset_instr(instruction_t *i, zval_t *a) {

    i->type = I_CIN_offset;

    i->first = malloc(sizeof(zval_t));
    zval_init(i->first);
    zval_copy(i->first, a); // offset
    i->second = NULL;
    i->third = NULL;

    return EOK;
}

#include "hugo.h"

#endif // INSTRUCTION_H
