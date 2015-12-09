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
    kv_init(vm->stack);

    kv_resize(instruction_t, vm->instructions, 50);
    kv_resize(zval_t, vm->stack, 100);

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

    for (size_t i = 0; i < kv_size(vm->stack); ++i) {
        zval_dispose(&kv_A(vm->stack, i));
    }

    kv_destroy(vm->instructions);
    kv_destroy(vm->stack);

    return EOK;
}

static inline void process_PUSH_instr(vm_t *vm, const int offset) {
    zval_t val;
    zval_set(&val, offset);
    kv_push(zval_t, vm->stack, val);
}

INLINED void process_COUT_pop_instr(vm_t *vm) {

    zval_t val = kv_A(vm->stack, kv_size(vm->stack) - 1);

    if (ZVAL_IS_DEFINED(&val)) {
        if (ZVAL_IS_INT(&val)) {
            printf("%d\n", zval_get_int(&val));
        } else if (ZVAL_IS_DOUBLE(&val)) {
            printf("%f\n", zval_get_double(&val));
        } else if (ZVAL_IS_STRING(&val)) {
            printf("%s\n", zval_get_string(&val));
        }
    } else {
        printf("[null]\n");
    }

    zval_dispose(&val);
}

result_t vm_exec(vm_t *vm) {

    result_t ret;
    vm->ip = 0;

    while (vm->ip < kv_size(vm->instructions)) {
        instruction_t *i = &kv_A(vm->instructions, vm->ip);
        debug_print("[0x%.8lu]: [%d]\n", vm->ip, i->type);

        switch (i->type) {
            case I_PUSH:
                process_PUSH_instr(vm, ZVAL_GET_INT(i->first));
                vm->ip++;
                break;
            case I_JMP:
                vm->ip = (size_t) ZVAL_GET_INT(i->first);
                break;
            case I_COUT_pop:
                process_COUT_pop_instr(vm);
                vm->ip++;
                break;
            case I_PUSH_zval: {
                zval_t val;
                zval_init(&val);
                zval_copy(&val, i->first);
                kv_push(zval_t, vm->stack, val);
                vm->ip++;
                break;
            }
            case I_POP_N: {
                for (int j = 0; j < ZVAL_GET_INT(i->first); ++j) {
                    zval_dispose(&kv_pop(vm->stack));
                }
                vm->ip++;
                break;
            }
            case I_POP_to: {
                kv_A(vm->stack, ZVAL_GET_INT(i->first)) = kv_pop(vm->stack);
                vm->ip++;
                break;
            }



            case I_ADD_zval: {
                zval_t res;
                if ((ret = zval_add(&res, i->first, i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_ADD_zval_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_add(&res, i->first, &b)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_ADD_pop_zval: {
                zval_t res;
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_add(&res, i->first, &a)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_ADD_pop: {
                zval_t res;
                zval_t a = kv_pop(vm->stack);
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_add(&res, &a, &b)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                zval_dispose(&a); zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            default:
                debug_print("%s [%d]\n", "Not implemented yet.", i->type);
                return ESYS;
        }

    };



    if (kv_size(vm->stack) > 0) {
        debug_print("\n\nStack is not cleaned properly %lu item(s) left there\n", kv_size(vm->stack));
    }

    return EOK;
}
