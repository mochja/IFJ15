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
#include "interpreter.h"

Describe(Interpreter)
BeforeEach(Interpreter) {}
AfterEach(Interpreter) {}

Ensure(Interpreter, should_add_two_integers) {
    klist_t(stack_list) *sl;
    sl = kl_init(stack_list);

    stack_item_t *add = calloc(1, sizeof(stack_item_t));
    add->type = I_ADD;

    zval_t item[3];
    add->first = &item[0];
    add->second = &item[1];
    add->third = &item[2];

    ZVAL_SET_INT(add->second, 2);
    ZVAL_SET_INT(add->third, 1);

    *kl_pushp(stack_list, sl) = add;

    interpret(sl);

    assert_true(ZVAL_IS_INT(add->first));
    assert_that(ZVAL_GET_INT(add->first), is_equal_to(3));

    free(add);
    kl_destroy(stack_list, sl);
}

Ensure(Interpreter, should_add_two_doubles) {
    klist_t(stack_list) *sl;
    sl = kl_init(stack_list);

    stack_item_t *add = calloc(1, sizeof(stack_item_t));
    add->type = I_ADD;

    zval_t item[3];
    add->first = &item[0];
    add->second = &item[1];
    add->third = &item[2];

    ZVAL_SET_DOUBLE(add->second, 2.33);
    ZVAL_SET_DOUBLE(add->third, 1.27);

    *kl_pushp(stack_list, sl) = add;

    interpret(sl);

    assert_true(ZVAL_IS_DOUBLE(add->first));
    assert_double_equal(ZVAL_GET_DOUBLE(add->first), 3.6);

    free(add);
    kl_destroy(stack_list, sl);
}

TestSuite *interpreter_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Interpreter, should_add_two_integers);
    add_test_with_context(suite, Interpreter, should_add_two_doubles);
    return suite;
}
