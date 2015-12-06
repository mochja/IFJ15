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
#include "mathi.h"

klist_t(instruction_list) *create_instructions_from_expression(klist_t(expr_stack) *expr) {
    klist_t(instruction_list) *instr;
    instr = kl_init(instruction_list);

    klist_t(expr_stack) *buff;
    buff = kl_init(expr_stack);

    int offset = 0;

    for (kliter_t(expr_stack) *it = kl_begin(expr); it != kl_end(expr); it = kl_next(it)) {
        expr_t *curr = kl_val(it);

        if (EXPR_IS_INT(curr)) {
            *kl_push(expr_stack, buff) = curr;
            offset++;
        } else if (EXPR_IS_OPERAND(curr)) {

            if (offset < 0) {
                fprintf(stderr, "Unknown expression");
                return instr;
            }

            expr_t *a, *b;

            if (kl_shift(expr_stack, buff, &b) == -1) {
                b = NULL;
            }

            if (kl_shift(expr_stack, buff, &a) == -1) {
                a = NULL;
            }

            if (EXPR_GET_OPERAND(curr) == Op_PLUS) {

                if ((a != NULL) && (b != NULL)) {

                    if (ZVAL_IS_INT(&a->val) && ZVAL_IS_INT(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_ADDI_int_instr(ZVAL_GET_INT(&a->val), ZVAL_GET_INT(&b->val));
                    } else

                    if (ZVAL_IS_DOUBLE(&a->val) && ZVAL_IS_DOUBLE(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_ADDD_double_instr(ZVAL_GET_DOUBLE(&a->val), ZVAL_GET_DOUBLE(&b->val));
                    }

                } else if ((a != NULL) && (b == NULL)) {

                    if (ZVAL_IS_INT(&a->val)) {
                        *kl_pushp(instruction_list, instr) = create_ADDI_int_pop_instr(ZVAL_GET_INT(&a->val));
                        offset--;
                    } else

                    if (ZVAL_IS_DOUBLE(&a->val)) {
                        *kl_pushp(instruction_list, instr) = create_ADDD_double_pop_instr(ZVAL_GET_DOUBLE(&a->val));
                        offset--;
                    }

                } else if ((a == NULL) && (b != NULL)) {

                    if (ZVAL_IS_INT(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_ADDI_pop_int_instr(ZVAL_GET_INT(&b->val));
                        offset--;
                    } else

                    if (ZVAL_IS_DOUBLE(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_ADDD_pop_double_instr(ZVAL_GET_DOUBLE(&b->val));
                        offset--;
                    }

                } else {
                    *kl_pushp(instruction_list, instr) = create_ADD_pop_instr();
                    offset -= 2;
                }
            } else
            if (EXPR_GET_OPERAND(curr) == Op_MINUS) {

                if ((a != NULL) && (b != NULL)) {

                    if (ZVAL_IS_INT(&a->val) && ZVAL_IS_INT(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_SUBI_int_instr(ZVAL_GET_INT(&a->val), ZVAL_GET_INT(&b->val));
                    } else

                    if (ZVAL_IS_DOUBLE(&a->val) && ZVAL_IS_DOUBLE(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_SUBD_double_instr(ZVAL_GET_DOUBLE(&a->val), ZVAL_GET_DOUBLE(&b->val));
                    }

                } else if ((a != NULL) && (b == NULL)) {

                    if (ZVAL_IS_INT(&a->val)) {
                        *kl_pushp(instruction_list, instr) = create_SUBI_int_pop_instr(ZVAL_GET_INT(&a->val));
                        offset--;
                    } else

                    if (ZVAL_IS_DOUBLE(&a->val)) {
                        *kl_pushp(instruction_list, instr) = create_SUBD_double_pop_instr(ZVAL_GET_DOUBLE(&a->val));
                        offset--;
                    }

                } else if ((a == NULL) && (b != NULL)) {

                    if (ZVAL_IS_INT(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_SUBI_pop_int_instr(ZVAL_GET_INT(&b->val));
                        offset--;
                    } else

                    if (ZVAL_IS_DOUBLE(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_SUBD_pop_double_instr(ZVAL_GET_DOUBLE(&b->val));
                        offset--;
                    }

                } else {
                    *kl_pushp(instruction_list, instr) = create_SUB_pop_instr();
                    offset -= 2;
                }
            } else
            if (EXPR_GET_OPERAND(curr) == Op_MUL) {

                if ((a != NULL) && (b != NULL)) {

                    if (ZVAL_IS_INT(&a->val) && ZVAL_IS_INT(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_MULI_int_instr(ZVAL_GET_INT(&a->val), ZVAL_GET_INT(&b->val));
                    } else

                    if (ZVAL_IS_DOUBLE(&a->val) && ZVAL_IS_DOUBLE(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_MULD_double_instr(ZVAL_GET_DOUBLE(&a->val), ZVAL_GET_DOUBLE(&b->val));
                    }

                } else if ((a != NULL) && (b == NULL)) {

                    if (ZVAL_IS_INT(&a->val)) {
                        *kl_pushp(instruction_list, instr) = create_MULI_int_pop_instr(ZVAL_GET_INT(&a->val));
                        offset--;
                    } else

                    if (ZVAL_IS_DOUBLE(&a->val)) {
                        *kl_pushp(instruction_list, instr) = create_MULD_double_pop_instr(ZVAL_GET_DOUBLE(&a->val));
                        offset--;
                    }

                } else if ((a == NULL) && (b != NULL)) {

                    if (ZVAL_IS_INT(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_MULI_pop_int_instr(ZVAL_GET_INT(&b->val));
                        offset--;
                    } else

                    if (ZVAL_IS_DOUBLE(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_MULD_pop_double_instr(ZVAL_GET_DOUBLE(&b->val));
                        offset--;
                    }

                } else {
                    *kl_pushp(instruction_list, instr) = create_MUL_pop_instr();
                    offset -= 2;
                }
            } else
            if (EXPR_GET_OPERAND(curr) == Op_DIV) {

                if ((a != NULL) && (b != NULL)) {

                    if (ZVAL_IS_INT(&a->val) && ZVAL_IS_INT(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_DIVI_int_instr(ZVAL_GET_INT(&a->val), ZVAL_GET_INT(&b->val));
                    } else

                    if (ZVAL_IS_DOUBLE(&a->val) && ZVAL_IS_DOUBLE(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_DIVD_double_instr(ZVAL_GET_DOUBLE(&a->val), ZVAL_GET_DOUBLE(&b->val));
                    }

                } else if ((a != NULL) && (b == NULL)) {

                    if (ZVAL_IS_INT(&a->val)) {
                        *kl_pushp(instruction_list, instr) = create_DIVI_int_pop_instr(ZVAL_GET_INT(&a->val));
                        offset--;
                    } else

                    if (ZVAL_IS_DOUBLE(&a->val)) {
                        *kl_pushp(instruction_list, instr) = create_DIVD_double_pop_instr(ZVAL_GET_DOUBLE(&a->val));
                        offset--;
                    }

                } else if ((a == NULL) && (b != NULL)) {

                    if (ZVAL_IS_INT(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_DIVI_pop_int_instr(ZVAL_GET_INT(&b->val));
                        offset--;
                    } else

                    if (ZVAL_IS_DOUBLE(&b->val)) {
                        *kl_pushp(instruction_list, instr) = create_DIVD_pop_double_instr(ZVAL_GET_DOUBLE(&b->val));
                        offset--;
                    }

                } else {
                    *kl_pushp(instruction_list, instr) = create_DIV_pop_instr();
                    offset -= 2;
                }
            }

        }
    }

    kl_destroy(expr_stack, buff);

    return instr;
}
