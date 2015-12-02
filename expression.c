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

#include "expression.h"
#include "globals.h"

static inline unsigned int get_operation(int type) {

    switch (type) {
        case PLUS:          return Op_PLUS;
        case MINUS:         return Op_MINUS;
        case MULTIPLY:      return Op_MUL;
        case DEVIDE:        return Op_DIV;
        case EQUALS:        return Op_EQ;
        case NOT_EQUAL:     return Op_NEQ;
        case LEFT_ARROW:    return Op_LESS;
        case RIGHT_ARROW:   return Op_MORE;
        case LESS_OR_EQUAL: return Op_LEQ;
        case MORE_OR_EQUAL: return Op_MEQ;
        case LEFT_CULUM:    return Op_LB;
        case RIGHT_CULUM:   return Op_RB;
        case ID:
        case INT:
        case DOUBLE:
        case STRING:        return Op_VAR;
        default:            return Op_ERR;
    }
}

static inline unsigned int get_rule(unsigned int left, unsigned int right) {
    return ( left > Op_DOL || right > Op_DOL ) ? ESYS : __table[left][right];
}

klist_t(expr_stack)* build_expression(klist_t(token_list) *tokens) {
    klist_t(expr_stack) *op_stack;
    klist_t(expr_stack) *stack;
    kliter_t(token_list) *p;

    op_stack = kl_init(expr_stack);
    stack = kl_init(expr_stack);

    for (p = kl_begin(tokens); p != kl_end(tokens); p = kl_next(p)) {
        token_t *t = kl_val(p);

        expr_t *exp = calloc(1, sizeof(expr_t));

        if (get_operation(t->type) == Op_VAR) {
            if (t->type == INT) {
                EXPR_SET_INT(exp, t->iVal);
            }
            *kl_pushp(expr_stack, stack) = exp;
        } else {
            EXPR_SET_OPERAND(exp, t->type);

            if (kl_begin(op_stack)) {
                expr_t *top = kl_begin(op_stack)->data;

                if ( get_rule(get_operation(EXPR_GET_OPERAND(top)), get_operation(t->type)) != L ) {
                    kl_shift(expr_stack, op_stack, kl_pushp(expr_stack, stack));
                } else {
                    *kl_pushp(expr_stack, op_stack) = exp;
                }
            } else {
                *kl_pushp(expr_stack, op_stack) = exp;
            }
        }
    }

    for (kliter_t(expr_stack) *it = kl_begin(op_stack); it != kl_end(op_stack); it = kl_next(it)) {
        kl_shift(expr_stack, op_stack, kl_pushp(expr_stack, op_stack));
    }

    kl_destroy(expr_stack, op_stack);

    return stack;
}
