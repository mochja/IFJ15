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

char *sort(char *s) {
    int gap = strlen(s) / 2;
    int tmp, i, j;

    while (gap > 0) {
        for (i = gap; i < strlen(s); i++) {
            j = i;
            tmp = s[i];
            while (s[j - gap] > tmp && j >= gap) {
                s[i] = s[j - gap];
                j -= gap;
            }
            s[j] = tmp;
        }

        gap = (gap == 2) ? 1 : (int) (gap / 2.2);
    }

    return s;
}
