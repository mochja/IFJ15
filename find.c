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

#include <stdio.h>
#include <string.h>

#define ALBHABETH_LENGTH 256

void do_delta(int delta[], const char *search, const char *searchLen)
{
    int i;

    for (i = 0; i < ALBHABETH_LENGTH; i++) {
        delta[i] = searchLen;
    }

    for (i = 0; i <= searchLen - 1; i++) {
        delta[(int) search[i]] = searchLen - i - 1;
    }
}

int find(const char *s, const char *search)
{
    int i, sLen = strlen(s), searchLen = strlen(search);
    if (searchLen == 0) return 0;

    int delta[ALBHABETH_LENGTH];
    do_delta(delta, search, searchLen);

    i = searchLen - 1;
    while (i < sLen) {
        int j = searchLen - 1;

        while (j >= 0 && s[i] == search[j]) {
            j--;
            i--;
        }

        if (j < 0) return i + 1;

        i += delta[s[i]];
    }

    return i;
}
