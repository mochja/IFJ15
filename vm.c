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
#include "ial.h"

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

        instruction_t *instr = &kl_val(it);

        if (instr->type == I_LABEL) {
            kv_a(size_t, labels, ZVAL_GET_INT(instr->first)) = i;
        } else {
            i++;
        }
    }

    i = 0;
    for (kliter_t(instruction_list) *it = kl_begin(code);
         it != kl_end(code); it = kl_next(it)) {

        instruction_t *instr = &kl_val(it);

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

//    for (size_t i = 0; i < kv_size(vm->call_stack); ++i) {
//        vm_ctx_dispose(&kv_A(vm->call_stack, i));
//    }

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
                } else if (ZVAL_IS_STRING(&top)) {
                    ret = ESEM2;
                    running = false;
                } else {
                    vm->ip++;
                }
                zval_dispose(&top);
                break;
            }
            case I_COUT_pop: {
                zval_t val = kv_A(vm->stack, kv_size(vm->stack) - 1);
                if ((ret = zval_print(&val)) != EOK) {
                    running = false;
                }
                zval_dispose(&val);
                vm->ip++;
                break;
            }
            case I_COUT_offset: {
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t val = kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first));
                if ((ret = zval_print(&val)) != EOK) {
                    running = false;
                }
                vm->ip++;
                break;
            }
            case I_COUT_zval: {
                if ((ret = zval_print(i->first)) != EOK) {
                    running = false;
                }
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
                if (kv_size(ctx->locals) <= ctx->nargs + ZVAL_GET_INT(i->first)) {
                    zval_init(&kv_a(zval_t, ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)));
                }
                zval_cast(&kv_a(zval_t, ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)), &val);
                vm->ip++;
                break;
            }
            case I_LOAD: {
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t *a = &kv_a(zval_t, ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first));
                zval_t copy;
                zval_init(&copy);
                zval_copy(&copy, a);
                kv_push(zval_t, vm->stack, copy);
                vm->ip++;
                break;
            }
            case I_CALL: {
                ctx_t ctx;
                vm_ctx_init(&ctx, vm->ip + 1, (unsigned int) ZVAL_GET_INT(i->second));

                for (int j = ZVAL_GET_INT(i->second); j > 0; j--) {
                    zval_t tmp = kv_pop(vm->stack);
                    if (!ZVAL_IS_DEFINED(&tmp)) {
                        debug_print("%s", "undefined variable passed as param.");
                        ret = ERUN2;
                        running = false;
                        break;
                    }
                    kv_a(zval_t, ctx.locals, j - 1) = tmp;
                }

                zval_t val;
                zval_init(&val);
                kv_push(zval_t, ctx.locals, val); // set as divider between args and locals

                kv_push(ctx_t, vm->call_stack, ctx);
                vm->ip = (size_t) ZVAL_GET_INT(i->first);
                break;
            }
            case I_CIN_offset: {

                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t *a = &kv_a(zval_t, ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first));

                if (ZVAL_IS_INT(a)) {
                    int t;
                    if (scanf("%d", &t) != EOF) {
                        zval_dispose(a);
                        zval_set(a, t);
                    }
                } else if (ZVAL_IS_DOUBLE(a)) {
                    float t;
                    if (scanf("%g", &t) != EOF) {
                        zval_dispose(a);
                        zval_set(a, (double) t);
                    }
                } else if (ZVAL_IS_STRING(a)) {
                    char buff[512];
                    if (scanf("%s", buff) != EOF) {
                        zval_dispose(a);
                        zval_set(a, buff);
                    }
                } else {
                    ret = ERUN1;
                    running = false;
                }

                vm->ip++;
                break;
            }
            case I_CALLN: {

                char *fn = zval_get_string(i->first);

                if (!strcmp("length", fn)) {
                    zval_t s = kv_pop(vm->stack);
                    zval_t res;
                    if (!ZVAL_IS_DEFINED(&s)) return ERUN1;
                    zval_set(&res, (int) length(zval_get_string(&s)));
                    zval_dispose(&s);
                    kv_push(zval_t, vm->stack, res);
                } else if (!strcmp("substr", fn)) {
                    zval_t count = kv_pop(vm->stack);
                    zval_t pos = kv_pop(vm->stack);
                    zval_t s = kv_pop(vm->stack);
                    zval_t res;
                    if (!ZVAL_IS_DEFINED(&count)) return ERUN1;
                    if (!ZVAL_IS_DEFINED(&pos)) return ERUN1;
                    if (!ZVAL_IS_DEFINED(&s)) return ERUN1;
                    char *r = substr(zval_get_string(&s), zval_get_int(&pos), zval_get_int(&count));
                    if (r == NULL) {
                        ret = ERUN3;
                        running = false;
                        break;
                    }
                    zval_set(&res, r);
                    free(r);
                    zval_dispose(&s);
                    zval_dispose(&count);
                    zval_dispose(&pos);
                    kv_push(zval_t, vm->stack, res);
                } else if (!strcmp("concat", fn)) {
                    zval_t s2 = kv_pop(vm->stack);
                    zval_t s1 = kv_pop(vm->stack);
                    if (!ZVAL_IS_DEFINED(&s2)) return ERUN1;
                    if (!ZVAL_IS_DEFINED(&s1)) return ERUN1;
                    zval_t res;
                    char *r = concat(zval_get_string(&s1), zval_get_string(&s2));
                    if (r == NULL) {
                        ret = ERUN3;
                        running = false;
                        break;
                    }
                    zval_set(&res, r);
                    free(r);
                    zval_dispose(&s1);
                    zval_dispose(&s2);
                    kv_push(zval_t, vm->stack, res);
                } else if (!strcmp("find", fn)) {
                    zval_t s2 = kv_pop(vm->stack);
                    zval_t s1 = kv_pop(vm->stack);
                    if (!ZVAL_IS_DEFINED(&s1)) return ERUN1;
                    if (!ZVAL_IS_DEFINED(&s2)) return ERUN1;
                    zval_t res;
                    zval_set(&res, find(zval_get_string(&s1), zval_get_string(&s2)));
                    zval_dispose(&s1);
                    zval_dispose(&s2);
                    kv_push(zval_t, vm->stack, res);
                } else if (!strcmp("sort", fn)) {
                    zval_t s1 = kv_pop(vm->stack);
                    if (!ZVAL_IS_DEFINED(&s1)) return ERUN1;
                    zval_t res;
                    char *r = sort(zval_get_string(&s1));
                    if (r == NULL) {
                        ret = ERUN3;
                        running = false;
                        break;
                    }
                    zval_set(&res, r);
                    free(r);
                    zval_dispose(&s1);
                    kv_push(zval_t, vm->stack, res);
                } else {
                    debug_print("%s: [%s]\n", "Unknown native fn", fn);
                }

                vm->ip++;
                break;
            }
            case I_RETURN: {
                if (!ZVAL_IS_DEFINED(&kv_top(vm->stack))) return ERUN1;
                ctx_t ctx = kv_pop(vm->call_stack);
                vm->ip = ctx.returnip;
                vm_ctx_dispose(&ctx);
                break;
            }
            case I_EXIT: {
                running = false;
                break;
            }

            // <<<GENERATED

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
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_ADD_zval_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_add(&res, i->first,
                                    &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }




            case I_ADD_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_add(&res, &a, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_ADD_pop_zval: {
                zval_t res;
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_add(&res, &a, i->first)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_ADD_pop_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_add(&res, &a,
                                    &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)))) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
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
            case I_ADD_offset_zval: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_add(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                    i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_ADD_offset_pop: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_add(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)), &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_SUB_zval: {
                zval_t res;
                if ((ret = zval_sub(&res, i->first, i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_SUB_zval_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_sub(&res, i->first, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_SUB_zval_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_sub(&res, i->first,
                                    &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }




            case I_SUB_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_sub(&res, &a, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_SUB_pop_zval: {
                zval_t res;
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_sub(&res, &a, i->first)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_SUB_pop_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_sub(&res, &a,
                                    &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)))) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }



            case I_SUB_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_sub(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                    &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_SUB_offset_zval: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_sub(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                    i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_SUB_offset_pop: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_sub(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)), &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_MUL_zval: {
                zval_t res;
                if ((ret = zval_mul(&res, i->first, i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_MUL_zval_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_mul(&res, i->first, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_MUL_zval_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_mul(&res, i->first,
                                    &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }




            case I_MUL_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_mul(&res, &a, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_MUL_pop_zval: {
                zval_t res;
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_mul(&res, &a, i->first)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_MUL_pop_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_mul(&res, &a,
                                    &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)))) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }



            case I_MUL_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_mul(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                    &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_MUL_offset_zval: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_mul(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                    i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_MUL_offset_pop: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_mul(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)), &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_DIV_zval: {
                zval_t res;
                if ((ret = zval_div(&res, i->first, i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_DIV_zval_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_div(&res, i->first, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_DIV_zval_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_div(&res, i->first,
                                    &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }




            case I_DIV_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_div(&res, &a, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_DIV_pop_zval: {
                zval_t res;
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_div(&res, &a, i->first)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_DIV_pop_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_div(&res, &a,
                                    &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)))) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }



            case I_DIV_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_div(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                    &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_DIV_offset_zval: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_div(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                    i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_DIV_offset_pop: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_div(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)), &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LT_zval: {
                zval_t res;
                if ((ret = zval_lt(&res, i->first, i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LT_zval_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_lt(&res, i->first, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LT_zval_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_lt(&res, i->first,
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }




            case I_LT_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_lt(&res, &a, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LT_pop_zval: {
                zval_t res;
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_lt(&res, &a, i->first)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LT_pop_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_lt(&res, &a,
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)))) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }



            case I_LT_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_lt(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LT_offset_zval: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_lt(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                   i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LT_offset_pop: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_lt(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)), &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GT_zval: {
                zval_t res;
                if ((ret = zval_gt(&res, i->first, i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GT_zval_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_gt(&res, i->first, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GT_zval_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_gt(&res, i->first,
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }




            case I_GT_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_gt(&res, &a, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GT_pop_zval: {
                zval_t res;
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_gt(&res, &a, i->first)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GT_pop_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_gt(&res, &a,
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)))) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }



            case I_GT_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_gt(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GT_offset_zval: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_gt(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                   i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GT_offset_pop: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_gt(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)), &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LE_zval: {
                zval_t res;
                if ((ret = zval_le(&res, i->first, i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LE_zval_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_le(&res, i->first, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LE_zval_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_le(&res, i->first,
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }




            case I_LE_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_le(&res, &a, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LE_pop_zval: {
                zval_t res;
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_le(&res, &a, i->first)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LE_pop_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_le(&res, &a,
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)))) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }



            case I_LE_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_le(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LE_offset_zval: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_le(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                   i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_LE_offset_pop: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_le(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)), &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GE_zval: {
                zval_t res;
                if ((ret = zval_ge(&res, i->first, i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GE_zval_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_ge(&res, i->first, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GE_zval_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_ge(&res, i->first,
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }




            case I_GE_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_ge(&res, &a, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GE_pop_zval: {
                zval_t res;
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_ge(&res, &a, i->first)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GE_pop_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_ge(&res, &a,
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)))) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }



            case I_GE_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_ge(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GE_offset_zval: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_ge(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                   i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_GE_offset_pop: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_ge(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)), &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_EQ_zval: {
                zval_t res;
                if ((ret = zval_eq(&res, i->first, i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_EQ_zval_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_eq(&res, i->first, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_EQ_zval_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_eq(&res, i->first,
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }




            case I_EQ_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_eq(&res, &a, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_EQ_pop_zval: {
                zval_t res;
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_eq(&res, &a, i->first)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_EQ_pop_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_eq(&res, &a,
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)))) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }



            case I_EQ_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_eq(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_EQ_offset_zval: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_eq(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                   i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_EQ_offset_pop: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_eq(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)), &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_NQ_zval: {
                zval_t res;
                if ((ret = zval_nq(&res, i->first, i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_NQ_zval_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_nq(&res, i->first, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_NQ_zval_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_nq(&res, i->first,
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }




            case I_NQ_pop: {
                zval_t res;
                zval_t b = kv_pop(vm->stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_nq(&res, &a, &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_NQ_pop_zval: {
                zval_t res;
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_nq(&res, &a, i->first)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_NQ_pop_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t a = kv_pop(vm->stack);
                if ((ret = zval_nq(&res, &a,
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)))) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&a);
                    return ret;
                }
                zval_dispose(&a);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }



            case I_NQ_offset: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_nq(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                   &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->second)))) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_NQ_offset_zval: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                if ((ret = zval_nq(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)),
                                   i->second)) != EOK) {
                    zval_dispose(&res);
                    return ret;
                }
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            case I_NQ_offset_pop: {
                zval_t res;
                ctx_t *ctx = &kv_top(vm->call_stack);
                zval_t b = kv_pop(vm->stack);
                if ((ret = zval_nq(&res, &kv_A(ctx->locals, ctx->nargs + ZVAL_GET_INT(i->first)), &b)) != EOK) {
                    zval_dispose(&res);
                    zval_dispose(&b);
                    return ret;
                }
                zval_dispose(&b);
                kv_push(zval_t, vm->stack, res);
                vm->ip++;
                break;
            }
            // GENERATED>>>>
            default:
                debug_print("%s [%d]\n", "Not implemented yet.", i->type);
                return ESYS;
        }
    };

    for (int i = 0; i < kv_size(vm->stack); ++i) {
        zval_dispose(&kv_A(vm->stack, i));
    }

    return ret;
}
