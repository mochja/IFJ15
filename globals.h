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

#ifndef GLOBALS_H_
#define GLOBALS_H_

typedef enum {
    EOK,
    ELEX,
    ESYN,
    ESEM,
    ESEM2,
    ESEM3,
    ESEM4,
    EEOF,
    ESYS = 99,
} result_t;

#define CHECK_FLAG(x, t)                (((x) & (t)) == (t))
#define HAS_FLAG(x, t)                  ((x) & (t))

#endif // GLOBALS_H_
