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
#include "vm.h"

result_t vm_init(vm_t *vm, klist_t(instruction_list) *instructions) {
    kv_init(vm->instructions);
    kv_init(vm->stack.data);

    kv_resize(instruction_t, vm->instructions, 50);
    kv_resize(zval_t, vm->stack.data, 100);

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
            instruction_copy(&kv_a(instruction_t, vm->instructions, i), instr);
            zval_set(kv_A(vm->instructions, i).first, kv_A(labels, ZVAL_GET_INT(instr->first)));
        } else if (instr->type != I_LABEL) {
            instruction_copy(&kv_a(instruction_t, vm->instructions, i), instr);
        }

        if (instr->type != I_LABEL && instr->type != I_NOOP) {
            i++;
        }
    }

    kv_destroy(labels);

    return EOK;
}

result_t vm_dispose(vm_t *vm) {

    for (size_t i = 0; i < kv_size(vm->instructions); i++) {
        instruction_dispose(&kv_A(vm->instructions, i));
    }

    for (size_t i = 0; i < kv_size(vm->stack.data); ++i) {
        zval_dispose(&kv_A(vm->stack.data, i));
    }

    kv_destroy(vm->instructions);
    kv_destroy(vm->stack.data);

    return EOK;
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

        default:
            return 0;
    }
}

result_t vm_exec(vm_t *vm) {
    size_t actual_addr = 0;

    while (actual_addr < kv_size(vm->instructions)) {
        instruction_t *i = &kv_A(vm->instructions, actual_addr);
        printf("[0x%.8lu]: [%d] \n", actual_addr, i->type);
        actual_addr = proccess_instruction(i, &vm->stack, actual_addr);
    };

    return EOK;
}
