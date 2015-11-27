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
 * LICENSE file in the root directory of this source tree.
 */

#include <cgreen/cgreen.h>
#include "interpreter.h"

Describe(Interpreter)
BeforeEach(Interpreter) {}
AfterEach(Interpreter) {}

Ensure(Interpreter, abcs) {
    klist_t(stack_list) *sl;
    sl = kl_init(stack_list);

    stack_item_t *add = calloc(1, sizeof(stack_item_t));
    add->type = I_ADD;

    zvar_t item[3];
    add->first = &item[0];
    add->second = &item[1];
    add->third = &item[2];

    add->second->dataType = T_INT;
    add->second->iVal = 1;
    add->third->dataType = T_INT;
    add->third->iVal = 2;

    *kl_pushp(stack_list, sl) = add;

    interpret(sl);

    assert_that(add->first->dataType, is_equal_to(T_INT));
    assert_that(add->first->iVal, is_equal_to(3));

    free(add);
    kl_destroy(stack_list, sl);
}

TestSuite *interpreter_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Interpreter, abcs);
    return suite;
}
