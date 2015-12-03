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

#ifndef SCANER_H_
#define SCANER_H_

#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"

typedef struct
{
  char* str;
  int length;
  int allocSize;
} string;

struct T_Token
{
    int type;
    union
    {
        char*   s;
        int     i;
        double  d;
    }data;
    tresult result;
};

FILE *source;
struct T_Token nextToken();

#endif // SCANER_H_
