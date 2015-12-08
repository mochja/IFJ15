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
#include "vm.h"
#include "mathi.h"

Describe(VM)
BeforeEach(VM) {}
AfterEach(VM) {}

Ensure(VM, should_replace_labels_with_addresses_on_initialize) {
    klist_t(instruction_list) *sl;
    sl = kl_init(instruction_list);

    *kl_pushp(instruction_list, sl) = create_JMP_instr(8);   // 0
    *kl_pushp(instruction_list, sl) = create_LABEL_instr(2);
    *kl_pushp(instruction_list, sl) = create_JMP_instr(1);   // 1
    *kl_pushp(instruction_list, sl) = create_LABEL_instr(8);
    *kl_pushp(instruction_list, sl) = create_JMP_instr(2);   // 2
    *kl_pushp(instruction_list, sl) = create_LABEL_instr(1);
    *kl_pushp(instruction_list, sl) = create_POP_instr(100); // 3

    vm_t vm;
    vm_init(&vm, sl);
    kl_destroy(instruction_list, sl);

    zval_t *first = kv_A(vm.instructions, 0).first;
    assert_true(ZVAL_IS_INT(first));
    assert_equal(ZVAL_GET_INT(first), 2);

    first = kv_A(vm.instructions, 1).first;
    assert_true(ZVAL_IS_INT(first));
    assert_equal(ZVAL_GET_INT(first), 3);

    first = kv_A(vm.instructions, 2).first;
    assert_true(ZVAL_IS_INT(first));
    assert_equal(ZVAL_GET_INT(first), 1);

    first = kv_A(vm.instructions, 3).first;
    assert_true(ZVAL_IS_INT(first));
    assert_equal(ZVAL_GET_INT(first), 100);

    vm_dispose(&vm);
}

Ensure(VM, should_add_two_integers) {
    klist_t(instruction_list) *sl;
    sl = kl_init(instruction_list);

    *kl_pushp(instruction_list, sl) = create_ADDI_int_instr(5, 2);
    *kl_pushp(instruction_list, sl) = create_ADDI_int_instr(5, 2);
    *kl_pushp(instruction_list, sl) = create_ADD_pop_instr();
    *kl_pushp(instruction_list, sl) = create_ADDI_int_pop_instr(5);
    *kl_pushp(instruction_list, sl) = create_ADDI_pop_int_instr(5);

    vm_t vm;
    vm_init(&vm, sl);
    kl_destroy(instruction_list, sl);

    vm_exec(&vm);

    assert_equal(kv_size(vm.stack.data), 1);
    assert_equal(ZVAL_GET_INT(&kv_A(vm.stack.data, 0)), 24);

    vm_dispose(&vm);
}

Ensure(VM, should_add_two_doubles) {
    klist_t(instruction_list) *sl;
    sl = kl_init(instruction_list);

    *kl_pushp(instruction_list, sl) = create_ADDD_double_instr(5.32, 5.28);
    *kl_pushp(instruction_list, sl) = create_ADDD_double_instr(3.12, 8.48);
    *kl_pushp(instruction_list, sl) = create_ADD_pop_instr();

    vm_t vm;
    vm_init(&vm, sl);
    kl_destroy(instruction_list, sl);

    vm_exec(&vm);

    assert_equal(kv_size(vm.stack.data), 1);
    assert_double_equal(ZVAL_GET_DOUBLE(&kv_A(vm.stack.data, 0)), 22.2);

    vm_dispose(&vm);
}

Ensure(VM, should_do_some_adv_math) {
    klist_t(instruction_list) *sl;
    sl = kl_init(instruction_list);

    *kl_pushp(instruction_list, sl) = create_MULI_int_instr(5, 5);   // 25
    *kl_pushp(instruction_list, sl) = create_ADDI_pop_int_instr(5);  // 30
    *kl_pushp(instruction_list, sl) = create_DIVI_int_instr(5, 10);  // 5 / 10 = 0.5
    *kl_pushp(instruction_list, sl) = create_MULI_pop_int_instr(10); // 0
    *kl_pushp(instruction_list, sl) = create_ADD_pop_instr();        // 30 + 0

    vm_t vm;
    vm_init(&vm, sl);
    kl_destroy(instruction_list, sl);

    vm_exec(&vm);

    assert_equal(kv_size(vm.stack.data), 1);
    assert_equal(ZVAL_GET_INT(&kv_A(vm.stack.data, 0)), 30);

    vm_dispose(&vm);
}

Ensure(VM, should_do_some_adv_math_on_doubles) {
    klist_t(instruction_list) *sl;
    sl = kl_init(instruction_list);

    *kl_pushp(instruction_list, sl) = create_MULD_double_instr(5, 5);   // 25
    *kl_pushp(instruction_list, sl) = create_ADDD_pop_double_instr(5);  // 30
    *kl_pushp(instruction_list, sl) = create_DIVD_double_instr(5, 10);  // 5 / 10 = 0.5
    *kl_pushp(instruction_list, sl) = create_MULD_pop_double_instr(10); // 0.5 * 10 = 5
    *kl_pushp(instruction_list, sl) = create_ADD_pop_instr();           // 30 + 5

    vm_t vm;
    vm_init(&vm, sl);
    kl_destroy(instruction_list, sl);

    vm_exec(&vm);

    assert_equal(kv_size(vm.stack.data), 1);
    assert_double_equal(ZVAL_GET_DOUBLE(&kv_A(vm.stack.data, 0)), 35);

    vm_dispose(&vm);
}

TestSuite *vm_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, VM, should_replace_labels_with_addresses_on_initialize);
    add_test_with_context(suite, VM, should_add_two_integers);
    add_test_with_context(suite, VM, should_add_two_doubles);
    add_test_with_context(suite, VM, should_do_some_adv_math);
    add_test_with_context(suite, VM, should_do_some_adv_math_on_doubles);
    return suite;
}
