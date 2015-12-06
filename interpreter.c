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
#include "interpreter.h"
#include "mathi.h"

interpreter_t *init_interpreter(klist_t(instruction_list) *instructions) {
    interpreter_t *intr = calloc(1, sizeof(interpreter_t));

    kv_init(intr->instructions);
    kv_init(intr->stack.data);

    kv_resize(instruction_t, intr->instructions, 50);
    kv_resize(zval_t, intr->stack.data, 100);

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
            zval_set(kv_A(intr->instructions, i).first, kv_A(labels, ZVAL_GET_INT(instr->first)));
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

    for (size_t i = 0; i < kv_size(intr->stack.data); ++i) {
        zval_t *val = &kv_A(intr->stack.data, i);

        if (val == NULL) {
            continue;
        }

        if (ZVAL_IS_STRING(val)) {
            free(ZVAL_GET_STRING(val));
        }
    }

    kv_destroy(intr->instructions);
    kv_destroy(intr->stack.data);
    free(intr);
}

typedef struct __stack_t stack_t;

static inline void process_PUSH_instr(stack_t *stack, const int offset) {
    zval_t val;
    zval_set(&val, offset);
    kv_push(zval_t, stack->data, val);
}

INLINED void process_COUT_pop_instr(stack_t *stack) {

    zval_t *val = &kv_pop(stack->data);

    if (ZVAL_IS_INT(val)) {
        printf("%d", zval_get_int(val));
    }
}

static size_t proccess_instruction(instruction_t *instr, struct __stack_t *stack, const size_t actual_addr) {

    switch (instr->type) {

        case I_PUSH:
            process_PUSH_instr(stack, ZVAL_GET_INT(instr->first));
            return actual_addr + 1;
        case I_JMP:
            return (size_t) ZVAL_GET_INT(instr->first);
        case I_COUT_pop:
            process_COUT_pop_instr(stack);
            return actual_addr + 1;

        case I_ADDI_int:
            process_ADDI_int_instr(stack, ZVAL_GET_INT(instr->first), ZVAL_GET_INT(instr->second));
            return actual_addr + 1;
        case I_ADDI_int_pop:
            process_ADDI_int_pop_instr(stack, ZVAL_GET_INT(instr->first));
            return actual_addr + 1;
        case I_ADDI_pop_int:
            process_ADDI_pop_int_instr(stack, ZVAL_GET_INT(instr->first));
            return actual_addr + 1;
        case I_ADD_pop:
            process_ADD_pop_instr(stack);
            return actual_addr + 1;
        case I_SUBI_int:
            process_SUBI_int_instr(stack, ZVAL_GET_INT(instr->first), ZVAL_GET_INT(instr->second));
            return actual_addr + 1;
        case I_SUBI_int_pop:
            process_SUBI_int_pop_instr(stack, ZVAL_GET_INT(instr->first));
            return actual_addr + 1;
        case I_SUBI_pop_int:
            process_SUBI_pop_int_instr(stack, ZVAL_GET_INT(instr->first));
            return actual_addr + 1;
        case I_SUB_pop:
            process_SUB_pop_instr(stack);
            return actual_addr + 1;
        case I_MULI_int:
            process_MULI_int_instr(stack, ZVAL_GET_INT(instr->first), ZVAL_GET_INT(instr->second));
            return actual_addr + 1;
        case I_MULI_int_pop:
            process_MULI_int_pop_instr(stack, ZVAL_GET_INT(instr->first));
            return actual_addr + 1;
        case I_MULI_pop_int:
            process_MULI_pop_int_instr(stack, ZVAL_GET_INT(instr->first));
            return actual_addr + 1;
        case I_MUL_pop:
            process_MUL_pop_instr(stack);
            return actual_addr + 1;
        case I_DIVI_int:
            process_DIVI_int_instr(stack, ZVAL_GET_INT(instr->first), ZVAL_GET_INT(instr->second));
            return actual_addr + 1;
        case I_DIVI_int_pop:
            process_DIVI_int_pop_instr(stack, ZVAL_GET_INT(instr->first));
            return actual_addr + 1;
        case I_DIVI_pop_int:
            process_DIVI_pop_int_instr(stack, ZVAL_GET_INT(instr->first));
            return actual_addr + 1;
        case I_DIV_pop:
            process_DIV_pop_instr(stack);
            return actual_addr + 1;
        case I_ADDD_double:
            process_ADDD_double_instr(stack, ZVAL_GET_DOUBLE(instr->first), ZVAL_GET_DOUBLE(instr->second));
            return actual_addr + 1;
        case I_ADDD_double_pop:
            process_ADDD_double_pop_instr(stack, ZVAL_GET_DOUBLE(instr->first));
            return actual_addr + 1;
        case I_ADDD_pop_double:
            process_ADDD_pop_double_instr(stack, ZVAL_GET_DOUBLE(instr->first));
            return actual_addr + 1;
        case I_SUBD_double:
            process_SUBD_double_instr(stack, ZVAL_GET_DOUBLE(instr->first), ZVAL_GET_DOUBLE(instr->second));
            return actual_addr + 1;
        case I_SUBD_double_pop:
            process_SUBD_double_pop_instr(stack, ZVAL_GET_DOUBLE(instr->first));
            return actual_addr + 1;
        case I_SUBD_pop_double:
            process_SUBD_pop_double_instr(stack, ZVAL_GET_DOUBLE(instr->first));
            return actual_addr + 1;
        case I_MULD_double:
            process_MULD_double_instr(stack, ZVAL_GET_DOUBLE(instr->first), ZVAL_GET_DOUBLE(instr->second));
            return actual_addr + 1;
        case I_MULD_double_pop:
            process_MULD_double_pop_instr(stack, ZVAL_GET_DOUBLE(instr->first));
            return actual_addr + 1;
        case I_MULD_pop_double:
            process_MULD_pop_double_instr(stack, ZVAL_GET_DOUBLE(instr->first));
            return actual_addr + 1;
        case I_DIVD_double:
            process_DIVD_double_instr(stack, ZVAL_GET_DOUBLE(instr->first), ZVAL_GET_DOUBLE(instr->second));
            return actual_addr + 1;
        case I_DIVD_double_pop:
            process_DIVD_double_pop_instr(stack, ZVAL_GET_DOUBLE(instr->first));
            return actual_addr + 1;
        case I_DIVD_pop_double:
            process_DIVD_pop_double_instr(stack, ZVAL_GET_DOUBLE(instr->first));
            return actual_addr + 1;
        default:
            return 0;
    }
}

void run_interpreter(interpreter_t *intr) {
    size_t actual_addr = 0;

    while (actual_addr < kv_size(intr->instructions)) {
        instruction_t *i = &kv_A(intr->instructions, actual_addr);
        printf("[0x%.8lu]: [%d] \n", actual_addr, i->type);
        actual_addr = proccess_instruction(i, &intr->stack, actual_addr);
    };
}
