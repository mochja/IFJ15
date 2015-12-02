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
#include "expression.h"

Describe(Expression)
BeforeEach(Expression) {}
AfterEach(Expression) {}

Ensure(Expression, can_contain_a_number) {
    expr_t *expr = calloc(1, sizeof(expr_t));

    EXPR_SET_INT(expr, 1);
    assert_true(EXPR_IS_INT(expr));
    assert_false(EXPR_IS_ZVAL(expr));
    assert_false(EXPR_IS_STRING(expr));
    assert_false(EXPR_IS_DOUBLE(expr));
    assert_false(EXPR_IS_OPERAND(expr));

    EXPR_SET_DOUBLE(expr, 1.2);
    assert_true(EXPR_IS_DOUBLE(expr));
    assert_false(EXPR_IS_INT(expr));
    assert_false(EXPR_IS_ZVAL(expr));
    assert_false(EXPR_IS_STRING(expr));
    assert_false(EXPR_IS_OPERAND(expr));

    free(expr);
}

Ensure(Expression, can_contain_a_operand) {
    expr_t *expr = calloc(1, sizeof(expr_t));
    EXPR_SET_OPERAND(expr, '+');

    assert_true(EXPR_IS_OPERAND(expr));
    assert_false(EXPR_IS_INT(expr));
    assert_false(EXPR_IS_ZVAL(expr));
    assert_false(EXPR_IS_STRING(expr));
    assert_false(EXPR_IS_DOUBLE(expr));

    free(expr);
}

Ensure(Expression, can_contain_a_zval_ptr) {
    expr_t *expr = calloc(1, sizeof(expr_t));
    zval_t *val = calloc(1, sizeof(zval_t));

    EXPR_SET_ZVAL(expr, val);

    assert_true(EXPR_IS_ZVAL(expr));
    assert_false(EXPR_IS_OPERAND(expr));
    assert_false(EXPR_IS_INT(expr));
    assert_false(EXPR_IS_STRING(expr));
    assert_false(EXPR_IS_DOUBLE(expr));

    free(val);
    free(expr);
}

// This one is maybe, bla bla
Ensure(Expression, can_contain_a_string) {
    expr_t *expr = calloc(1, sizeof(expr_t));

    EXPR_SET_STRING(expr, "string");

    assert_true(EXPR_IS_STRING(expr));
    assert_false(EXPR_IS_ZVAL(expr));
    assert_false(EXPR_IS_OPERAND(expr));
    assert_false(EXPR_IS_INT(expr));
    assert_false(EXPR_IS_DOUBLE(expr));

    free(expr);
}

Ensure(Expression, should_be_able_to_generate_expr_stack) {
    // a+b*c-d

//    expr_t expr1, expr2, expr3, expr4, expr5;
//
//    EXPR_SET_INT(&expr1, 1);
//    EXPR_SET_OPERAND(&expr2, '+');
//    EXPR_SET_INT(&expr3, 3);
//    EXPR_SET_OPERAND(&expr4, '*');
//    EXPR_SET_INT(&expr5, 5);

    token_t t1, t2, t3, t4, t5;

    klist_t(token_list) *l = kl_init(token_list);
    *kl_pushp(token_list, l) = &t1;
    *kl_pushp(token_list, l) = &t2;
    *kl_pushp(token_list, l) = &t3;
    *kl_pushp(token_list, l) = &t4;
    *kl_pushp(token_list, l) = &t5;

    klist_t(expr_stack) *s;

    s = build_expression(l);

    int i = 0;
    for (kliter_t(expr_stack) *it = kl_begin(s); it != kl_end(s); it = kl_next(it)) {
        i++;
    }

    assert_that(i, is_equal_to(5));
}

TestSuite *expression_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Expression, can_contain_a_number);
    add_test_with_context(suite, Expression, can_contain_a_operand);
    add_test_with_context(suite, Expression, can_contain_a_zval_ptr);
    add_test_with_context(suite, Expression, can_contain_a_string);
    add_test_with_context(suite, Expression, should_be_able_to_generate_expr_stack);
    return suite;
}
