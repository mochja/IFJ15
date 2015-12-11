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
    expr_init(expr);

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

    expr_dispose(expr);
    free(expr);
}

Ensure(Expression, can_contain_a_operand) {
    expr_t *expr = calloc(1, sizeof(expr_t));
    expr_init(expr);

    EXPR_SET_OPERAND(expr, '+');

    assert_true(EXPR_IS_OPERAND(expr));
    assert_false(EXPR_IS_INT(expr));
    assert_false(EXPR_IS_STRING(expr));
    assert_false(EXPR_IS_DOUBLE(expr));

    expr_dispose(expr);
    free(expr);
}

// This one is maybe, bla bla
Ensure(Expression, can_contain_a_string) {
    expr_t *expr = calloc(1, sizeof(expr_t));
    expr_init(expr);


    EXPR_SET_STRING(expr, "string");

    assert_true(EXPR_IS_STRING(expr));
    assert_false(EXPR_IS_OPERAND(expr));
    assert_false(EXPR_IS_INT(expr));
    assert_false(EXPR_IS_DOUBLE(expr));

    expr_dispose(expr);
    free(expr);
}

TestSuite *expression_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Expression, can_contain_a_number);
    add_test_with_context(suite, Expression, can_contain_a_operand);
    add_test_with_context(suite, Expression, can_contain_a_string);
    add_test_with_context(suite, Expression, should_be_able_to_generate_expr_stack);
    return suite;
}
