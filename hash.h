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

#ifndef HASH_H_
#define HASH_H_

#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#define MAX_HTSIZE 101

#define uint32_t unsigned int

struct hItem {
    struct hItem *next;

    char *name;
    int dataType;

    union {
        int iVal;
        double dVal;
        char *sVal;
    };

    bool isDefined;
    int params;
    int paramPosition;
    int f_label;
};

typedef struct hItem hTabItem;

typedef struct {
    int size;
    hTabItem *array[];
} tTable;

tTable *initHashTable(int size);
hTabItem *createNewItem();
hTabItem *searchItem(const tTable *t, const char *name);
void insertHashTable(tTable *t, hTabItem *newItem);
void freeHashTable(tTable *t);

#endif  // HASH_H_
