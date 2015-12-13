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

result_t vm_init(vm_t *vm, klist_t(instruction_list) *code) {

    kv_init(vm->code);
    kv_init(vm->stack);

    kv_resize(instruction_t, vm->code, 5);
    kv_resize(zval_t, vm->stack, 20);

    kvec_t(size_t) labels;
    kv_init(labels);
    kv_resize(size_t, labels, 50);

    kv_init(vm->call_stack);
    kv_resize(ctx_t, vm->call_stack, 10);

    size_t i = 0;
    for (kliter_t(instruction_list) *it = kl_begin(code);
            it != kl_end(code); it = kl_next(it)) {

        instruction_t *instr = kl_val(it);

        if (instr->type == I_LABEL) {
            kv_a(size_t, labels, ZVAL_GET_INT(instr->first)) = i;
        } else {
            i++;
        }
    }

    i = 0;
    for (kliter_t(instruction_list) *it = kl_begin(code);
         it != kl_end(code); it = kl_next(it)) {

        instruction_t *instr = kl_val(it);

        if (instr->type == I_JMP || instr->type == I_CALL || instr->type == I_JMPE) {
            instruction_t tmp;
            instruction_copy(&tmp, instr);
            kv_a(instruction_t, vm->code, i) = tmp;
            zval_set(kv_A(vm->code, i).first, kv_A(labels, ZVAL_GET_INT(instr->first)));
        } else if (instr->type != I_LABEL) {
            instruction_t tmp;
            instruction_copy(&tmp, instr);
            kv_a(instruction_t, vm->code, i) = tmp;
        }

        if (instr->type != I_LABEL && instr->type != I_NOOP) {
            i++;
        }
    }

    kv_destroy(labels);

    return EOK;
}



INLINED result_t vm_ctx_init(ctx_t *ctx, size_t ip, unsigned int nargs) {

    kv_init(ctx->locals);
    kv_resize(zval_t, ctx->locals, 5);

    ctx->nargs = nargs;
    ctx->returnip = ip;
    return EOK;
}



INLINED result_t vm_ctx_dispose(ctx_t *ctx) {

    for (size_t i = 0; i < kv_size(ctx->locals); ++i) {
        zval_dispose(&kv_A(ctx->locals, i));
    }

    kv_destroy(ctx->locals);

    return EOK;
}



result_t vm_dispose(vm_t *vm) {

    for (size_t i = 0; i < kv_size(vm->code); i++) {
        instruction_dispose(&kv_A(vm->code, i));
    }

    for (size_t i = 0; i < kv_size(vm->stack); ++i) {
        zval_dispose(&kv_A(vm->stack, i));
    }

    kv_destroy(vm->code);
    kv_destroy(vm->stack);

    for (size_t i = 0; i < kv_size(vm->call_stack); ++i) {
        vm_ctx_dispose(&kv_A(vm->call_stack, i));
    }

    kv_destroy(vm->call_stack);

    return EOK;
}



static inline void process_PUSH_instr(vm_t *vm, const int offset) {
    zval_t val;
    zval_set(&val, offset);
    kv_push(zval_t, vm->stack, val);
}



result_t vm_exec(vm_t *vm) {

    result_t ret = EOK;
    vm->ip = 0;

    bool running = true;

    while (running) {

        if (vm->ip >= kv_size(vm->code)) {
            return ERUN3;
        }

        instruction_t *i = &kv_A(vm->code, vm->ip);
        char bufr[256];
        instruction_print(bufr, i);
        debug_print("[0x%.8lu]: %s\n", vm->ip, bufr);

        switch (i->type) {
            case I_PUSH:
                process_PUSH_instr(vm, ZVAL_GET_INT(i->first));
                vm->ip++;
                break;
            case I_JMP:
                vm->ip = (size_t) zval_get_int(i->first);
                break;
            case I_JMPE: {
                zval_t top = kv_pop(vm->stack);
                zval_t res;
                zval_eq(&res, &top, i->second);
                if (ZVAL_IS_DEFINED(&res) && ZVAL_IS_INT(&res) && (zval_get_int(&res) == 1)) {
                    vm->ip = (size_t) zval_get_int(i->first);
                } else {
                    vm->ip++;
                }
                zval_dispose(&top);
                break;
            }
            case I_COUT_pop: {
                zval_t val = kv_A(vm->stack, kv_size(vm->stack) - 1);
                zval_print(&val);
                zval_dispose(&val);
                vm->ip++;
                break;
            }
            case I_COUT_offset: {
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t val = kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first));
                zval_print(&val);
                vm->ip++;
                break;
            }
            case I_PUSH_zval: {
                zval_t val;
                zval_init(&val);
                zval_copy(&val, i->first);
                kv_push(zval_t, vm->stack, val);
                vm->ip++;
                break;
            }
            case I_STORE_zval: {
                zval_t val;
                zval_init(&val);
                zval_copy(&val, i->first);
                kv_push(zval_t, kv_top(vm->call_stack).locals, val);
                vm->ip++;
                break;
            }
            case I_POP_N: {
                for (int j = 0; j < ZVAL_GET_INT(i->first); j++) {
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
            case I_STORE: {
                zval_t val = kv_pop(vm->stack);
                ctx_t *ctx = &kv_top(vm->call_stack);
                kv_a(zval_t, ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)) = val;
                vm->ip++;
                break;
            }
            case I_LOAD: {
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t *a = &kv_a(zval_t, ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first));
                zval_t copy;
                zval_copy(&copy, a);
                kv_push(zval_t, vm->stack, copy);
                vm->ip++;
                break;
            }
            case I_CALL: {
                ctx_t ctx;
                vm_ctx_init(&ctx, vm->ip + 1, (unsigned int) ZVAL_GET_INT(i->second));

                for (int j = 0; j < ZVAL_GET_INT(i->second); j++) {
                    kv_push(zval_t, ctx.locals, kv_pop(vm->stack));
                }

                zval_t val;
                zval_init(&val);
                kv_push(zval_t, ctx.locals, val); // set as divider between args and locals

                kv_push(ctx_t, vm->call_stack, ctx);
                vm->ip = (size_t) ZVAL_GET_INT(i->first);
                break;
            }
            case I_RETURN: {
                if (kv_size(vm->call_stack) == 0) {
                    ret = ERUN3;
                    running = false;
                    break;
                }
                ctx_t ctx = kv_pop(vm->call_stack);
                vm->ip = ctx.returnip;
                kv_destroy(ctx.locals);
                break;
            }
            case I_EXIT: {
                running = false;
                break;
            }


            case I_GT: {
                zval_t res;
                if ((ret = zval_gt(&res, i->first, i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
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
            case I_ADD_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_add(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                    &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            default:
                debug_print("%s [%d]\n", "Not implemented yet.", i->type);
                return ESYS;
        }
    };

    if (kv_size(vm->stack) > 1) {
        debug_print("\n\nStack is not cleaned properly %lu items left there\n", kv_size(vm->stack));

        for (int i = 0; i < kv_size(vm->stack); ++i) {
            zval_print(&kv_A(vm->stack, i));
        }
    }

    return ret;
}
