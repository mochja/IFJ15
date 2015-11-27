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

#include "interpreter.h"
#include <stdio.h>

static inline int __add_int(const int a, const int b) {
    return a + b;
}

static inline double __add_double(const double a, const double b) {
    return a + b;
}

void interpret(klist_t(stack_list) *stack) {
    stack_item_t *item;

    while(kl_shift(stack_list, stack, &item) != -1) {
        switch(item->type) {
            case I_ADD:
                if (ZVAL_IS_INT(item->second) && ZVAL_IS_INT(item->third)) {
                    ZVAL_SET_INT(item->first, __add_int(item->second->iVal, item->third->iVal));
                } else if (ZVAL_IS_DOUBLE(item->second) && ZVAL_IS_DOUBLE(item->third)) {
                    ZVAL_SET_DOUBLE(item->first, __add_double(item->second->dVal, item->third->dVal));
                }
                break;
            default:
                printf("ERR: %d\n", item->type);
                break;
        }
    }
}

