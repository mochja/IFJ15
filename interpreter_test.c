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

Ensure(Interpreter, should_replace_labels_with_addresses_on_initialize) {
    klist_t(instruction_list) *sl;
    sl = kl_init(instruction_list);

    *kl_pushp(instruction_list, sl) = create_JMP_instr(8);   // 0
    *kl_pushp(instruction_list, sl) = create_LABEL_instr(2);
    *kl_pushp(instruction_list, sl) = create_JMP_instr(1);   // 1
    *kl_pushp(instruction_list, sl) = create_LABEL_instr(8);
    *kl_pushp(instruction_list, sl) = create_JMP_instr(2);   // 2
    *kl_pushp(instruction_list, sl) = create_LABEL_instr(1);
    *kl_pushp(instruction_list, sl) = create_POP_instr(100); // 3


    interpreter_t *intr = init_interpreter(sl);
    kl_destroy(instruction_list, sl);

    zval_t *fval = kv_A(intr->instructions, 0).first;
    assert_true(ZVAL_IS_INT(fval));
    assert_equal(ZVAL_GET_INT(fval), 2);

    fval = kv_A(intr->instructions, 1).first;
    assert_true(ZVAL_IS_INT(fval));
    assert_equal(ZVAL_GET_INT(fval), 3);

    fval = kv_A(intr->instructions, 2).first;
    assert_true(ZVAL_IS_INT(fval));
    assert_equal(ZVAL_GET_INT(fval), 1);

    fval = kv_A(intr->instructions, 3).first;
    assert_true(ZVAL_IS_INT(fval));
    assert_equal(ZVAL_GET_INT(fval), 100);

    destroy_interpreter(intr);
}

Ensure(Interpreter, should_add_two_integers) {
    klist_t(instruction_list) *sl;
    sl = kl_init(instruction_list);

    *kl_pushp(instruction_list, sl) = create_ADD_int_instr(5, 2);

    interpreter_t *intr = init_interpreter(sl);
    kl_destroy(instruction_list, sl);

    run_interpreter(intr);

    assert_equal(kv_size(intr->stack.data), 1);
    assert_equal(ZVAL_GET_INT(&kv_A(intr->stack.data, 0)), 7);

    destroy_interpreter(intr);
}

Ensure(Interpreter, should_add_two_doubles) {
    klist_t(instruction_list) *sl;
    sl = kl_init(instruction_list);

    instruction_t *add = calloc(1, sizeof(instruction_t));
    add->type = I_ADD_int;

    ZVAL_INIT_DOUBLE(add->second, 2.33);
    ZVAL_INIT_DOUBLE(add->third, 1.27);

    ZVAL_INIT_INT(add->first, /* offset */ 1);

    *kl_pushp(instruction_list, sl) = add;

//    interpret(sl);

    kl_destroy(instruction_list, sl);
}

TestSuite *interpreter_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Interpreter, should_replace_labels_with_addresses_on_initialize);
    add_test_with_context(suite, Interpreter, should_add_two_integers);
    add_test_with_context(suite, Interpreter, should_add_two_doubles);
    return suite;
}
