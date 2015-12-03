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
    instruction_t *pop = create_POP_instr(5);

    assert_true(ZVAL_IS_INT(pop->first));
    assert_equal(ZVAL_GET_INT(pop->first), 5);

    destroy_instruction(pop);
}

TestSuite *instruction_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Instruction, should_create_POP_instruction);
    return suite;
}
