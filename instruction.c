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

klist_t(instruction_list) *create_instructions_from_expression(klist_t(expr_stack) *expr) {
    klist_t(instruction_list) *instr;
    instr = kl_init(instruction_list);

    klist_t(expr_stack) *buff;
    buff = kl_init(expr_stack);

    for (kliter_t(expr_stack) *it = kl_begin(expr); it != kl_end(expr); it = kl_next(it)) {
        expr_t *curr = kl_val(it);

        if (EXPR_IS_INT(curr)) {
            *kl_push(expr_stack, buff) = curr;
        } else if (EXPR_IS_OPERAND(curr)) {
            expr_t *a, *b;
            kl_shift(expr_stack, buff, &b);
            kl_shift(expr_stack, buff, &a);

            if (EXPR_GET_OPERAND(curr) == Op_PLUS) {
                if (ZVAL_IS_INT(&a->val) && ZVAL_IS_INT(&b->val)) {
                    *kl_pushp(instruction_list, instr) = create_ADD_int_instr(ZVAL_GET_INT(&a->val), ZVAL_GET_INT(&b->val));
                }
            }
        }
    }

    kl_destroy(expr_stack, buff);

    return instr;
}
