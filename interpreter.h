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

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include "klist.h"
#include "zval.h"
#include "instruction.h"
#include "kvec.h"

typedef struct __interpreter_t interpreter_t;

struct __stack_t {
    kvec_t(zval_t) data;
    size_t base_pointer;
};

struct __interpreter_t {
    kvec_t(instruction_t) instructions;
    struct __stack_t stack;
};

void destroy_interpreter(interpreter_t *intr);
interpreter_t *init_interpreter(klist_t(instruction_list) *instructions);
void run_interpreter(interpreter_t *intr);

#endif // INTERPRETER_H_
