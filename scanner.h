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

#ifndef SCANNER_H_
#define SCANNER_H_

#include <stdio.h>
#include <ctype.h>
#include "globals.h"
#include "token.h"
#include "string.h"

typedef struct {
    char *source;
    size_t line;
    size_t pos;
} scanner_t;

result_t init_scanner(scanner_t *s, char *source);
result_t scanner_get_next_token(scanner_t *s, token_t *dest);
void scanner_dispose(scanner_t *s);

#endif // SCANNER_H_
