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

#ifndef VM_H_
#define VM_H_

#include "klist.h"
#include "zval.h"
#include "instruction.h"
#include "kvec.h"

typedef struct __vm_t vm_t;
typedef struct __ctx_t ctx_t;

struct __vm_t {
    kvec_t(instruction_t) code;
    kvec_t(zval_t) stack;
    kvec_t(ctx_t) call_stack;
    size_t ip; // instruction pointer
};

struct __ctx_t {
    kvec_t(zval_t) locals;
    size_t returnip;
    unsigned int nargs;
};

result_t vm_init(vm_t *vm, klist_t(instruction_list) *code);
result_t vm_dispose(vm_t *vm);

result_t vm_exec(vm_t *vm);

#endif // VM_H_
