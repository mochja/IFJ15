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
#include "instruction.h"

Describe(Instruction)
BeforeEach(Instruction) {}
AfterEach(Instruction) {}

Ensure(Instruction, should_create_POP_instruction) {
    instruction_t pop;
    create_POP_instr(&pop, 5);

    assert_true(ZVAL_IS_INT(pop.first));
    assert_equal(ZVAL_GET_INT(pop.first), 5);

    instruction_dispose(&pop);
}

//Ensure(Instruction, should_create_instruction_list_from_expression) {
//    klist_t(expr_stack) *expr;
//    expr = kl_init(expr_stack);
//
//    expr_t e1, e2, e3;
//    EXPR_SET_INT(&e1, 2);
//    EXPR_SET_INT(&e2, 5);
//    EXPR_SET_OPERAND(&e3, Op_PLUS);
//
//    *kl_pushp(expr_stack, expr) = &e1;
//    *kl_pushp(expr_stack, expr) = &e2;
//    *kl_pushp(expr_stack, expr) = &e3;
//
//    klist_t(instruction_list) *l = create_instructions_from_expression(expr);
//    kl_destroy(expr_stack, expr);
//
//    instruction_t *instr;
//    kl_shift(instruction_list, l, &instr);
//
//    assert_equal(instr->type, I_ADDI_int);
//    assert_equal(ZVAL_GET_INT(instr->first), 2);
//    assert_equal(ZVAL_GET_INT(instr->second), 5);
//
//    kl_destroy(instruction_list, l);
//}

TestSuite *instruction_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Instruction, should_create_POP_instruction);
//    add_test_with_context(suite, Instruction, should_create_instruction_list_from_expression);
    return suite;
}
