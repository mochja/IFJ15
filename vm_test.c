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

Describe(VM)
BeforeEach(VM) {}
AfterEach(VM) {}

Ensure(VM, should_replace_labels_with_addresses_on_initialize) {
    klist_t(instruction_list) *sl = kl_init(instruction_list);

    instruction_t jmp, lbl;

    create_JMP_instr(&jmp, 8);
    create_LABEL_instr(&lbl, 2);
    *kl_pushp(instruction_list, sl) = jmp;   // 0
    *kl_pushp(instruction_list, sl) = lbl;

    create_JMP_instr(&jmp, 1);
    create_LABEL_instr(&lbl, 8);
    *kl_pushp(instruction_list, sl) = jmp;   // 1
    *kl_pushp(instruction_list, sl) = lbl;

    create_JMP_instr(&jmp, 2);
    create_LABEL_instr(&lbl, 1);
    *kl_pushp(instruction_list, sl) = jmp;   // 2
    *kl_pushp(instruction_list, sl) = lbl;

    create_POP_instr(&lbl, 100);
    *kl_pushp(instruction_list, sl) = lbl; // 3

    vm_t vm;
    vm_init(&vm, sl);
    kl_destroy(instruction_list, sl);

    zval_t *first = kv_A(vm.code, 0).first;
    assert_true(ZVAL_IS_INT(first));
    assert_equal(ZVAL_GET_INT(first), 2);

    first = kv_A(vm.code, 1).first;
    assert_true(ZVAL_IS_INT(first));
    assert_equal(ZVAL_GET_INT(first), 3);

    first = kv_A(vm.code, 2).first;
    assert_true(ZVAL_IS_INT(first));
    assert_equal(ZVAL_GET_INT(first), 1);

    first = kv_A(vm.code, 3).first;
    assert_true(ZVAL_IS_INT(first));
    assert_equal(ZVAL_GET_INT(first), 100);

    vm_dispose(&vm);
}

Ensure(VM, should_add_two_integers) {

    klist_t(instruction_list) *sl = kl_init(instruction_list);
    zval_t a, b;

    instruction_t i;

    zval_set(&a, 5); zval_set(&b, 2);
    create_ADD_zval_instr(&i, &a, &b);
    *kl_pushp(instruction_list, sl) = i;

    create_ADD_zval_instr(&i, &a, &b);
    *kl_pushp(instruction_list, sl) = i;

    create_ADD_pop_instr(&i);
    *kl_pushp(instruction_list, sl) = i;

    vm_t vm;
    vm_init(&vm, sl);
    kl_destroy(instruction_list, sl);

    vm_exec(&vm);

    assert_equal(kv_size(vm.stack), 1);
    assert_equal(ZVAL_GET_INT(&kv_A(vm.stack, 0)), 14);

    vm_dispose(&vm);
}

TestSuite *vm_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, VM, should_replace_labels_with_addresses_on_initialize);
    add_test_with_context(suite, VM, should_add_two_integers);
    return suite;
}
