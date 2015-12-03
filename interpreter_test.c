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
    klist_t(instruction_list) *sl;
    sl = kl_init(instruction_list);

    instruction_t *add = calloc(1, sizeof(instruction_t));

    add->type = I_ADD;

    ZVAL_INIT_INT(add->second, 2);
    ZVAL_INIT_INT(add->third, 1);

    ZVAL_INIT_INT(add->first, /* offset */ 1);

    *kl_pushp(instruction_list, sl) = add;

    interpret(sl);

    kl_destroy(instruction_list, sl);
}

Ensure(Interpreter, should_add_two_doubles) {
    klist_t(instruction_list) *sl;
    sl = kl_init(instruction_list);

    instruction_t *add = calloc(1, sizeof(instruction_t));
    add->type = I_ADD;

    ZVAL_INIT_DOUBLE(add->second, 2.33);
    ZVAL_INIT_DOUBLE(add->third, 1.27);

    ZVAL_INIT_INT(add->first, /* offset */ 1);

    *kl_pushp(instruction_list, sl) = add;

    interpret(sl);

    kl_destroy(instruction_list, sl);
}

TestSuite *interpreter_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Interpreter, should_add_two_integers);
    add_test_with_context(suite, Interpreter, should_add_two_doubles);
    return suite;
}
