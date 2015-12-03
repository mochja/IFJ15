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

#include <stdio.h>
#include <string.h>
#include "interpreter.h"

interpreter_t *init_interpreter(klist_t(instruction_list) *instructions) {
    interpreter_t *intr = calloc(1, sizeof(interpreter_t));

    kv_init(intr->instructions);
    kv_init(intr->zstack);

    kv_resize(instruction_t, intr->instructions, 50);
    kv_resize(zval_t, intr->zstack, 100);

    kvec_t(size_t) labels;
    kv_init(labels);
    kv_resize(size_t, labels, 50);

    size_t i = 0;
    for (kliter_t(instruction_list) *it = kl_begin(instructions);
            it != kl_end(instructions); it = kl_next(it)) {

        instruction_t *instr = kl_val(it);

        if (instr->type == I_LABEL) {
            kv_a(size_t, labels, ZVAL_GET_INT(instr->first)) = i;
        } else {
            i++;
        }
    }

    i = 0;
    for (kliter_t(instruction_list) *it = kl_begin(instructions);
         it != kl_end(instructions); it = kl_next(it)) {

        instruction_t *instr = kl_val(it);

        if (instr->type == I_JMP) {
            copy_instruction(&kv_a(instruction_t, intr->instructions, i), instr);
            ZVAL_SET_INT(kv_A(intr->instructions, i).first, kv_A(labels, ZVAL_GET_INT(instr->first)));
        } else if (instr->type != I_LABEL) {
            copy_instruction(&kv_a(instruction_t, intr->instructions, i), instr);
        }

        if (instr->type != I_LABEL && instr->type != I_NOOP) {
            i++;
        }
    }

    kv_destroy(labels);

    return intr;
}

void destroy_interpreter(interpreter_t *intr) {

    for (size_t i = 0; i < kv_size(intr->instructions); i++) {
        destroy_zval(kv_A(intr->instructions, i).first);
        destroy_zval(kv_A(intr->instructions, i).second);
        destroy_zval(kv_A(intr->instructions, i).third);
    }

    for (size_t i = 0; i < kv_size(intr->zstack); ++i) {
        zval_t *val = &kv_A(intr->zstack, i);
        if (ZVAL_IS_STRING(val)) {
            free(ZVAL_GET_STRING(val));
        }
    }

    kv_destroy(intr->instructions);
    kv_destroy(intr->zstack);
    free(intr);
}

static inline int __add_int(const int a, const int b) {
    return a + b;
}

static inline double __add_double(const double a, const double b) {
    return a + b;
}

void interpret(klist_t(instruction_list) *instructions) {
    instruction_t *item;

    zval_t stack[50000];
    int basePtr = 0;
    int stackPtr = 0;

    while(kl_shift(instruction_list, instructions, &item) != -1) {
        switch(item->type) {
            case I_ADD:
                if (ZVAL_IS_INT(item->second) && ZVAL_IS_INT(item->third)) {
                    ZVAL_SET_INT(item->first, __add_int(item->second->iVal, item->third->iVal));
                } else if (ZVAL_IS_DOUBLE(item->second) && ZVAL_IS_DOUBLE(item->third)) {
                    ZVAL_SET_DOUBLE(item->first, __add_double(item->second->dVal, item->third->dVal));
                }
                break;
            case I_POP:
                stack[basePtr + ZVAL_GET_INT(item->first)] = stack[stackPtr];
                break;
            default:
                printf("ERR: %d\n", item->type);
                break;
        }
    }
}
