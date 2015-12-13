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
    klist_t(expr_stack) *buff = kl_init(expr_stack);
    int offset = 0;

    for (kliter_t(expr_stack) *it = kl_begin(expr); it != kl_end(expr); it = kl_next(it)) {
        expr_t *curr = &kl_val(it);

        if (EXPR_IS_INT(curr) || EXPR_IS_DOUBLE(curr) || EXPR_IS_STRING(curr) || EXPR_IS_OFFSET(curr)) {
            expr_t copy;
            expr_init(&copy);
            if ((ret = expr_copy(&copy, curr)) != EOK) {
                debug_print("%s\n", "<");
                return ret;
            }
            *kl_push(expr_stack, buff) = copy;
            offset++;
        } else if (EXPR_IS_OPERAND(curr)) {

            if (((unsigned int)EXPR_GET_INT(curr) == Op_LB) || ((unsigned int)EXPR_GET_INT(curr) == Op_RB)) continue;

            expr_t t1, t2;
            expr_t *a, *b;

            if (kl_shift(expr_stack, buff, &t1) == -1) {
                b = NULL;
            } else {
                b = &t1;
            }

            if (kl_shift(expr_stack, buff, &t2) == -1) {
                a = NULL;
            } else {
                a = &t2;
            }

            switch ((unsigned int) EXPR_GET_INT(curr)) {
                // <<<<GENERATED
                case Op_PLUS: {

                    instruction_t i;

                    if (a != NULL && b != NULL) {
                        if (EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_ADD_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a) && !EXPR_IS_OFFSET(b)) {
                            if ((ret = create_ADD_offset_zval_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (!EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_ADD_zval_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if ((ret = create_ADD_zval_instr(&i, &a->val, &b->val)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset++;
                    } else if (a == NULL && b != NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(b)) {
                            if ((ret = create_ADD_pop_zval_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(b)) {
                            if ((ret = create_ADD_pop_offset_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (a != NULL && b == NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(a)) {
                            if ((ret = create_ADD_zval_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a)) {
                            if ((ret = create_ADD_offset_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (offset > 1) {
                        if ((ret = create_ADD_pop_instr(&i)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset--;
                    } else {
                        debug_print("%s\n", "<");
                        kl_destroy(expr_stack, buff);
                        return ESYN; // Todo: fix error code
                    }

                    *kl_pushp(instruction_list, dest) = i;
                    break;
                }

                case Op_MINUS: {

                    instruction_t i;

                    if (a != NULL && b != NULL) {
                        if (EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_SUB_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a) && !EXPR_IS_OFFSET(b)) {
                            if ((ret = create_SUB_offset_zval_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (!EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_SUB_zval_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if ((ret = create_SUB_zval_instr(&i, &a->val, &b->val)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset++;
                    } else if (a == NULL && b != NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(b)) {
                            if ((ret = create_SUB_pop_zval_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(b)) {
                            if ((ret = create_SUB_pop_offset_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (a != NULL && b == NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(a)) {
                            if ((ret = create_SUB_zval_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a)) {
                            if ((ret = create_SUB_offset_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (offset > 1) {
                        if ((ret = create_SUB_pop_instr(&i)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset--;
                    } else {
                        debug_print("%s\n", "<");
                        kl_destroy(expr_stack, buff);
                        return ESYN; // Todo: fix error code
                    }

                    *kl_pushp(instruction_list, dest) = i;
                    break;
                }

                case Op_MUL: {

                    instruction_t i;

                    if (a != NULL && b != NULL) {
                        if (EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_MUL_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a) && !EXPR_IS_OFFSET(b)) {
                            if ((ret = create_MUL_offset_zval_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (!EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_MUL_zval_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if ((ret = create_MUL_zval_instr(&i, &a->val, &b->val)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset++;
                    } else if (a == NULL && b != NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(b)) {
                            if ((ret = create_MUL_pop_zval_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(b)) {
                            if ((ret = create_MUL_pop_offset_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (a != NULL && b == NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(a)) {
                            if ((ret = create_MUL_zval_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a)) {
                            if ((ret = create_MUL_offset_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (offset > 1) {
                        if ((ret = create_MUL_pop_instr(&i)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset--;
                    } else {
                        debug_print("%s\n", "<");
                        kl_destroy(expr_stack, buff);
                        return ESYN; // Todo: fix error code
                    }

                    *kl_pushp(instruction_list, dest) = i;
                    break;
                }

                case Op_DIV: {

                    instruction_t i;

                    if (a != NULL && b != NULL) {
                        if (EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_DIV_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a) && !EXPR_IS_OFFSET(b)) {
                            if ((ret = create_DIV_offset_zval_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (!EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_DIV_zval_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if ((ret = create_DIV_zval_instr(&i, &a->val, &b->val)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset++;
                    } else if (a == NULL && b != NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(b)) {
                            if ((ret = create_DIV_pop_zval_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(b)) {
                            if ((ret = create_DIV_pop_offset_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (a != NULL && b == NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(a)) {
                            if ((ret = create_DIV_zval_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a)) {
                            if ((ret = create_DIV_offset_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (offset > 1) {
                        if ((ret = create_DIV_pop_instr(&i)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset--;
                    } else {
                        debug_print("%s\n", "<");
                        kl_destroy(expr_stack, buff);
                        return ESYN; // Todo: fix error code
                    }

                    *kl_pushp(instruction_list, dest) = i;
                    break;
                }

                case Op_LESS: {

                    instruction_t i;

                    if (a != NULL && b != NULL) {
                        if (EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_LT_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a) && !EXPR_IS_OFFSET(b)) {
                            if ((ret = create_LT_offset_zval_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (!EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_LT_zval_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if ((ret = create_LT_zval_instr(&i, &a->val, &b->val)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset++;
                    } else if (a == NULL && b != NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(b)) {
                            if ((ret = create_LT_pop_zval_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(b)) {
                            if ((ret = create_LT_pop_offset_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (a != NULL && b == NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(a)) {
                            if ((ret = create_LT_zval_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a)) {
                            if ((ret = create_LT_offset_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (offset > 1) {
                        if ((ret = create_LT_pop_instr(&i)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset--;
                    } else {
                        debug_print("%s\n", "<");
                        kl_destroy(expr_stack, buff);
                        return ESYN; // Todo: fix error code
                    }

                    *kl_pushp(instruction_list, dest) = i;
                    break;
                }

                case Op_MORE: {

                    instruction_t i;

                    if (a != NULL && b != NULL) {
                        if (EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_GT_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a) && !EXPR_IS_OFFSET(b)) {
                            if ((ret = create_GT_offset_zval_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (!EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_GT_zval_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if ((ret = create_GT_zval_instr(&i, &a->val, &b->val)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset++;
                    } else if (a == NULL && b != NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(b)) {
                            if ((ret = create_GT_pop_zval_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(b)) {
                            if ((ret = create_GT_pop_offset_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (a != NULL && b == NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(a)) {
                            if ((ret = create_GT_zval_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a)) {
                            if ((ret = create_GT_offset_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (offset > 1) {
                        if ((ret = create_GT_pop_instr(&i)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset--;
                    } else {
                        debug_print("%s\n", "<");
                        kl_destroy(expr_stack, buff);
                        return ESYN; // Todo: fix error code
                    }

                    *kl_pushp(instruction_list, dest) = i;
                    break;
                }

                case Op_LEQ: {

                    instruction_t i;

                    if (a != NULL && b != NULL) {
                        if (EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_LE_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a) && !EXPR_IS_OFFSET(b)) {
                            if ((ret = create_LE_offset_zval_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (!EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_LE_zval_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if ((ret = create_LE_zval_instr(&i, &a->val, &b->val)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset++;
                    } else if (a == NULL && b != NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(b)) {
                            if ((ret = create_LE_pop_zval_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(b)) {
                            if ((ret = create_LE_pop_offset_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (a != NULL && b == NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(a)) {
                            if ((ret = create_LE_zval_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a)) {
                            if ((ret = create_LE_offset_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (offset > 1) {
                        if ((ret = create_LE_pop_instr(&i)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset--;
                    } else {
                        debug_print("%s\n", "<");
                        kl_destroy(expr_stack, buff);
                        return ESYN; // Todo: fix error code
                    }

                    *kl_pushp(instruction_list, dest) = i;
                    break;
                }

                case Op_MEQ: {

                    instruction_t i;

                    if (a != NULL && b != NULL) {
                        if (EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_GE_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a) && !EXPR_IS_OFFSET(b)) {
                            if ((ret = create_GE_offset_zval_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (!EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_GE_zval_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if ((ret = create_GE_zval_instr(&i, &a->val, &b->val)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset++;
                    } else if (a == NULL && b != NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(b)) {
                            if ((ret = create_GE_pop_zval_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(b)) {
                            if ((ret = create_GE_pop_offset_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (a != NULL && b == NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(a)) {
                            if ((ret = create_GE_zval_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a)) {
                            if ((ret = create_GE_offset_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (offset > 1) {
                        if ((ret = create_GE_pop_instr(&i)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset--;
                    } else {
                        debug_print("%s\n", "<");
                        kl_destroy(expr_stack, buff);
                        return ESYN; // Todo: fix error code
                    }

                    *kl_pushp(instruction_list, dest) = i;
                    break;
                }

                case Op_EQ: {

                    instruction_t i;

                    if (a != NULL && b != NULL) {
                        if (EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_EQ_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a) && !EXPR_IS_OFFSET(b)) {
                            if ((ret = create_EQ_offset_zval_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (!EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_EQ_zval_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if ((ret = create_EQ_zval_instr(&i, &a->val, &b->val)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset++;
                    } else if (a == NULL && b != NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(b)) {
                            if ((ret = create_EQ_pop_zval_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(b)) {
                            if ((ret = create_EQ_pop_offset_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (a != NULL && b == NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(a)) {
                            if ((ret = create_EQ_zval_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a)) {
                            if ((ret = create_EQ_offset_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (offset > 1) {
                        if ((ret = create_EQ_pop_instr(&i)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset--;
                    } else {
                        debug_print("%s\n", "<");
                        kl_destroy(expr_stack, buff);
                        return ESYN; // Todo: fix error code
                    }

                    *kl_pushp(instruction_list, dest) = i;
                    break;
                }

                case Op_NEQ: {

                    instruction_t i;

                    if (a != NULL && b != NULL) {
                        if (EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_NQ_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a) && !EXPR_IS_OFFSET(b)) {
                            if ((ret = create_NQ_offset_zval_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (!EXPR_IS_OFFSET(a) && EXPR_IS_OFFSET(b)) {
                            if ((ret = create_NQ_zval_offset_instr(&i, &a->val, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if ((ret = create_NQ_zval_instr(&i, &a->val, &b->val)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset++;
                    } else if (a == NULL && b != NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(b)) {
                            if ((ret = create_NQ_pop_zval_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(b)) {
                            if ((ret = create_NQ_pop_offset_instr(&i, &b->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (a != NULL && b == NULL && offset > 0) {
                        if (!EXPR_IS_OFFSET(a)) {
                            if ((ret = create_NQ_zval_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else if (EXPR_IS_OFFSET(a)) {
                            if ((ret = create_NQ_offset_pop_instr(&i, &a->val)) != EOK) {
                                debug_print("%s\n", "<");
                                kl_destroy(expr_stack, buff);
                                return ret;
                            }
                        } else {
                            debug_print("%s\n", "what is goin on there ?");
                            kl_destroy(expr_stack, buff);
                            return ESYN; // Todo: fix error code
                        }
                    } else if (offset > 1) {
                        if ((ret = create_NQ_pop_instr(&i)) != EOK) {
                            debug_print("%s\n", "<");
                            kl_destroy(expr_stack, buff);
                            return ret;
                        }
                        offset--;
                    } else {
                        debug_print("%s\n", "<");
                        kl_destroy(expr_stack, buff);
                        return ESYN; // Todo: fix error code
                    }

                    *kl_pushp(instruction_list, dest) = i;
                    break;
                }

                default: { debug_print("%s\n", "<"); return ESYS; }
                // >>>>>>GENERATED
            }

//            if (a != NULL) {
//                expr_dispose(a);
//            }
//
//            if (b != NULL) {
//                expr_dispose(b);
//            }
        }
    }

    for (kliter_t(expr_stack) *it = kl_begin(buff); it != kl_end(buff); it = kl_next(it)) {

        if (it != kl_begin(buff)) {
            return ESYN;
        }

        instruction_t i;
        expr_t exp = kl_val(it);

        if (EXPR_IS_OFFSET(&exp)) {
            if ((ret = create_LOAD_instr(&i, &exp.val)) != EOK) {
                return ret;
            }
        } else if (!EXPR_IS_OPERAND(&exp)) {
            if ((ret = create_PUSH_zval_instr(&i, &exp.val)) != EOK) {
                return ret;
            }
        }

        *kl_pushp(instruction_list, dest) = i;
    }

    kl_destroy(expr_stack, buff);

    return EOK;
}
