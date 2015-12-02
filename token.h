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

#ifndef TOKEN_H_
#define TOKEN_H_

#include "klist.h"

typedef struct __token_t token_t;

struct __token_t {
    int type;
    int result;

    union {
        int iVal;
        double dVal;
        char *sVal;
    };
};

#define __token_t_free(x)
KLIST_INIT(token_list, token_t*, __token_t_free)

#endif // TOKEN_H_
