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

#include "instruction.h"

/**
 * Move first element from stack to [addr_offset]
 */
static inline instruction_t* create_POP_instr(int addr_offset) {
    instruction_t *i = malloc(sizeof(instruction_t));

    zval_t *val = malloc(sizeof(zval_t));
    ZVAL_SET_INT(val, addr_offset);

    i->type = I_POP;
    i->first = val;

    return i;
}
