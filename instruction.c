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

#include "instruction.h"

result_t append_instr_from_expr(klist_t(instruction_list) *dest, klist_t(expr_stack) *expr) {
    result_t ret;
    klist_t(expr_stack) *buff;
    buff = kl_init(expr_stack);

    int offset = 0;

    for (kliter_t(expr_stack) *it = kl_begin(expr); it != kl_end(expr); it = kl_next(it)) {
        expr_t *curr = kl_val(it);

        if (EXPR_IS_INT(curr) || EXPR_IS_DOUBLE(curr) || EXPR_IS_OFFSET(curr)) {
            *kl_push(expr_stack, buff) = curr;
        } else if (EXPR_IS_OPERAND(curr)) {

            if ((EXPR_GET_INT(curr) == Op_LB) || (EXPR_GET_INT(curr) == Op_RB)) continue;

            expr_t *a, *b;

            if (kl_shift(expr_stack, buff, &b) == -1) {
                b = NULL;
            }

            if (kl_shift(expr_stack, buff, &a) == -1) {
                a = NULL;
            }

            if (a != NULL && b != NULL) {
                instruction_t *i = malloc(sizeof(instruction_t));
                if (i == NULL) return ESYS;

                if ((ret = create_ADD_zval_instr(i, &a->val, &b->val)) != EOK) {
                    free(i); return ret;
                }

                if (EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                    if ((ret = create_ADD_offset_instr(i, &a->val, &b->val)) != EOK) {
                        free(i); return ret;
                    }
                } else if ((ret = create_ADD_zval_instr(i, &a->val, &b->val)) != EOK) {
                    free(i); return ret;
                }

                *kl_pushp(instruction_list, dest) = i;
                offset++;
            } else if (a == NULL && b != NULL && offset > 0) {
                instruction_t *i = malloc(sizeof(instruction_t));
                if (i == NULL) return ESYS;
                if ((ret = create_ADD_pop_zval_instr(i, &b->val)) != EOK) {
                    free(i); return ret;
                }
                *kl_pushp(instruction_list, dest) = i;
                offset++;
            } else if (a != NULL && b == NULL && offset > 0) {
                instruction_t *i = malloc(sizeof(instruction_t));
                if (i == NULL) return ESYS;
                if ((ret = create_ADD_zval_pop_instr(i, &a->val)) != EOK) {
                    free(i); return ret;
                }
                *kl_pushp(instruction_list, dest) = i;
                offset++;
            } else if (offset > 1) {
                instruction_t *i = malloc(sizeof(instruction_t));
                if (i == NULL) return ESYS;
                if ((ret = create_ADD_pop_instr(i)) != EOK) {
                    free(i); return ret;
                }
                *kl_pushp(instruction_list, dest) = i;
                offset--; // remove 2, adds one
            }
        }
    }

    for (kliter_t(expr_stack) *it = kl_begin(buff); it != kl_end(buff); it = kl_next(it)) {
        instruction_t *i = malloc(sizeof(instruction_t));

        if ((ret = create_PUSH_zval_instr(i, &kl_val(it)->val)) != EOK) {
            free(i); return ret;
        }

        *kl_pushp(instruction_list, dest) = i;
    }

    kl_destroy(expr_stack, buff);

    return EOK;
}
