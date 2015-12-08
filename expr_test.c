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

#include <cgreen/cgreen.h>
#include "expr.h"
#include "globals.h"

Describe(Expression)
BeforeEach(Expression) {}
AfterEach(Expression) {}

Ensure(Expression, can_contain_a_number) {
    expr_t *expr = calloc(1, sizeof(expr_t));

    EXPR_SET_INT(expr, 1);
    assert_true(EXPR_IS_INT(expr));
    assert_false(EXPR_IS_STRING(expr));
    assert_false(EXPR_IS_DOUBLE(expr));
    assert_false(EXPR_IS_OPERAND(expr));

    EXPR_SET_DOUBLE(expr, 1.2);
    assert_true(EXPR_IS_DOUBLE(expr));
    assert_false(EXPR_IS_INT(expr));
    assert_false(EXPR_IS_STRING(expr));
    assert_false(EXPR_IS_OPERAND(expr));

    free(expr);
}

Ensure(Expression, can_contain_a_operand) {
    expr_t *expr = calloc(1, sizeof(expr_t));
    EXPR_SET_OPERAND(expr, '+');

    assert_true(EXPR_IS_OPERAND(expr));
    assert_false(EXPR_IS_INT(expr));
    assert_false(EXPR_IS_STRING(expr));
    assert_false(EXPR_IS_DOUBLE(expr));

    free(expr);
}

// This one is maybe, bla bla
Ensure(Expression, can_contain_a_string) {
    expr_t *expr = calloc(1, sizeof(expr_t));

    EXPR_SET_STRING(expr, "string");

    assert_true(EXPR_IS_STRING(expr));
    assert_false(EXPR_IS_OPERAND(expr));
    assert_false(EXPR_IS_INT(expr));
    assert_false(EXPR_IS_DOUBLE(expr));

    free(expr);
}

/**
 * 1+3*5-9 -> 1 3 5 * + 9 - -> 1 15 + 9 -> 16 9 - -> 7 | 3 + 1 * ( 9 + 5 ) ->
 */
Ensure(Expression, should_be_able_to_generate_expr_stack) {
    token_t t1, t2, t3, t4, t5, t6, t7;

    TOKEN_SET_CONST_INT(&t1, 1);

    t2.type = SMBL_TYPE;
    t2.flags = PLUS_SMBL;

    TOKEN_SET_CONST_INT(&t3, 3);

    t4.type = SMBL_TYPE;
    t4.flags = MULTIPLY_SMBL;

    TOKEN_SET_CONST_INT(&t5, 5);

    t6.type = SMBL_TYPE;
    t6.flags = MINUS_SMBL;

    TOKEN_SET_CONST_INT(&t7, 9);

    klist_t(token_list) *l = kl_init(token_list);
    *kl_pushp(token_list, l) = &t1;
    *kl_pushp(token_list, l) = &t2;
    *kl_pushp(token_list, l) = &t3;
    *kl_pushp(token_list, l) = &t4;
    *kl_pushp(token_list, l) = &t5;
    *kl_pushp(token_list, l) = &t6;
    *kl_pushp(token_list, l) = &t7;

    klist_t(expr_stack) *s = expression_from_tokens(NULL, l);
    kliter_t(expr_stack) *it = kl_begin(s);
    expr_t *expr;

    expr = kl_val(it);
    assert_true(EXPR_IS_INT(expr));
    assert_equal(EXPR_GET_INT(expr), 1);
    free(expr); it = kl_next(it);

    expr = kl_val(it);
    assert_true(EXPR_IS_INT(expr));
    assert_equal(EXPR_GET_INT(expr), 3);
    free(expr); it = kl_next(it);

    expr = kl_val(it);
    assert_true(EXPR_IS_INT(expr));
    assert_equal(EXPR_GET_INT(expr), 5);
    free(expr); it = kl_next(it);

    expr = kl_val(it);
    assert_true(EXPR_IS_OPERAND(expr));
    assert_equal(EXPR_GET_OPERAND(expr), Op_MUL);
    free(expr); it = kl_next(it);

    expr = kl_val(it);
    assert_true(EXPR_IS_OPERAND(expr));
    assert_equal(EXPR_GET_OPERAND(expr), Op_PLUS);
    free(expr); it = kl_next(it);

    expr = kl_val(it);
    assert_true(EXPR_IS_INT(expr));
    assert_equal(EXPR_GET_INT(expr), 9);
    free(expr); it = kl_next(it);

    expr = kl_val(it);
    assert_true(EXPR_IS_OPERAND(expr));
    assert_equal(EXPR_GET_OPERAND(expr), Op_MINUS);
    free(expr);

    // We should end up at end of parse_list
    assert_true(kl_next(it) == kl_end(s));

    kl_destroy(token_list, l);
    kl_destroy(expr_stack, s);
}

TestSuite *expression_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Expression, can_contain_a_number);
    add_test_with_context(suite, Expression, can_contain_a_operand);
    add_test_with_context(suite, Expression, can_contain_a_string);
    add_test_with_context(suite, Expression, should_be_able_to_generate_expr_stack);
    return suite;
}
