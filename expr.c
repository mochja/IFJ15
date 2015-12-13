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
#include "expr.h"
#include "globals.h"




result_t expr_init(expr_t *expr) {

    expr->flags = 0;
    return zval_init(&expr->val);
}



result_t expr_dispose(expr_t *expr) {

    if (expr == NULL) return EOK;

    expr->flags = 0;
    zval_dispose(&expr->val);

    return EOK;
}



result_t expr_copy(expr_t *dest, expr_t *src) {

    if (src == NULL) return EOK;

    dest->flags = src->flags;

    return zval_copy(&dest->val, &src->val);
}




static inline unsigned int get_operation(unsigned char type, unsigned int flags) {

    if (CHECK_FLAG(type, ID_TYPE) || (CHECK_FLAG(type, CONST_TYPE) && (HAS_FLAG(flags, INT_CONST|DOUBLE_CONST|TEXT_CONST)))) {
        return Op_VAR;
    }

    if (!CHECK_FLAG(type, SMBL_TYPE)) {
        return Op_ERR;
    }

    if (CHECK_FLAG(flags, PLUS_SMBL)) return Op_PLUS;
    if (CHECK_FLAG(flags, MINUS_SMBL)) return Op_MINUS;
    if (CHECK_FLAG(flags, MULTIPLY_SMBL)) return Op_MUL;
    if (CHECK_FLAG(flags, DEVIDE_SMBL)) return Op_DIV;
    if (CHECK_FLAG(flags, EQUALS_SMBL)) return Op_EQ;
    if (CHECK_FLAG(flags, NOT_EQUAL_SMBL)) return Op_NEQ;
    if (CHECK_FLAG(flags, LEFT_ARROW_SMBL)) return Op_LESS;
    if (CHECK_FLAG(flags, RIGHT_ARROW_SMBL)) return Op_MORE;
    if (CHECK_FLAG(flags, LESS_OR_EQUAL_SMBL)) return Op_LEQ;
    if (CHECK_FLAG(flags, MORE_OR_EQUAL_SMBL)) return Op_MEQ;
    if (CHECK_FLAG(flags, LEFT_CULUM_SMBL)) return Op_LB;
    if (CHECK_FLAG(flags, RIGHT_CULUM_SMBL)) return Op_RB;

    return Op_ERR;
}




static inline unsigned int get_rule(unsigned int left, unsigned int right) {
    return ( left > Op_DOL || right > Op_DOL ) ? ESYS : __table[left][right];
}




result_t expr_from_tokens(klist_t(expr_stack) *expr, klist_t(token_list) *tokens) {
    klist_t(expr_stack) *op_stack = kl_init(expr_stack);
    kliter_t(token_list) *p;

    for (p = kl_begin(tokens); p != kl_end(tokens); p = kl_next(p)) {
        token_t *t = kl_val(p);

        expr_t *exp = calloc(1, sizeof(expr_t));
        expr_init(exp);

        if (get_operation(t->type, t->flags) == Op_VAR) {

            if (TOKEN_HAS_TFLAG(t, CONST_TYPE, INT_CONST)) {
                EXPR_SET_INT(exp, ZVAL_GET_INT(&t->data));
            } else if (TOKEN_HAS_TFLAG(t, CONST_TYPE, DOUBLE_CONST)) {
                EXPR_SET_DOUBLE(exp, ZVAL_GET_DOUBLE(&t->data));
            } else if (TOKEN_HAS_TFLAG(t, ID_TYPE, OFFSET_ID)) {
                EXPR_SET_OFFSET(exp, ZVAL_GET_INT(&t->data));
            } else {
                kl_destroy(expr_stack, op_stack);
                free(exp);
                return ESYN;
            }

            *kl_pushp(expr_stack, expr) = exp;
        } else {
            unsigned int op = get_operation(t->type, t->flags);
            if (op == Op_ERR) {
                kl_destroy(expr_stack, op_stack);
                free(exp);
                return ESYN;
            }
            EXPR_SET_OPERAND(exp, op);

            while (kl_begin(op_stack) != kl_end(op_stack)) {
                expr_t *top = kl_val(kl_begin(op_stack));

                if (get_rule((unsigned int) EXPR_GET_OPERAND(top), op) == M) {
                    expr_t *a;
                    kl_shift(expr_stack, op_stack, &a);
                    *kl_pushp(expr_stack, expr) = a;
                } else {
                    break;
                }
            }

            *kl_push(expr_stack, op_stack) = exp;
        }
    }

    for (kliter_t(expr_stack) *it = kl_begin(op_stack); it != kl_end(op_stack); it = kl_next(it)) {
        expr_t *exp;
        if (kl_shift(expr_stack, op_stack, &exp) != -1) {
            expr_t *copy = malloc(sizeof(expr_t));
            expr_copy(copy, exp);
            *kl_pushp(expr_stack, expr) = copy;
        }
    }

    kl_destroy(expr_stack, op_stack);

    return EOK;
}
