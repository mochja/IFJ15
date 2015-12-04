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

#ifndef STRING_H_
#define STRING_H_

#include <stdlib.h>
#include <string.h>
#include "globals.h"

#define STR_ALLOC_SIZE      32

typedef struct {
    size_t length;
    size_t allocSize;
    char *str;
} string_t;

INLINED result_t initString(string_t *s) {

    if((s->str = malloc(STR_ALLOC_SIZE)) == NULL)
        return ESYS;

    s->str[0] = '\0';
    s->length = 0;
    s->allocSize = STR_ALLOC_SIZE;

    return EOK;
}

INLINED result_t strAdd(string_t *s, const char c) {

    if (s->length+1 >= s->allocSize) {
        if ((s->str = realloc(s->str, s->length + STR_ALLOC_SIZE)) == NULL) {
            free(s);
            return ESYS;
        }

        s->allocSize = s->length + STR_ALLOC_SIZE;
    }

    s->str[s->length] = c;
    s->str[++s->length] = '\0';
}

#endif // STRING_H_
