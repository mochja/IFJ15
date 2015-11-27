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

#include "hash.h"

tTable *initHashTable(int size)
{
    tTable *t;

    if ((t = calloc(1, sizeof(tTable) + sizeof(struct hItem) * size)) == NULL) {
        return NULL;
    }

    t->size = size;

    return t;
}

void freeHashTable(tTable *t)
{
    if (t == NULL) {
        return;
    }

    for (int i = 0; i < t->size; ++i) {
        hTabItem *freeItem;

        freeItem = t->array[i];
        while (true) {
            if (t->array[i] == NULL || t->array[i]->next == NULL) {
                break;
            } else {
                freeItem = t->array[i]->next;
                t->array[i]->next = freeItem->next;
                free(freeItem);
            }
        }
        free(t->array[i]);
    }
    free(t);
}

/*!
 * Hash Index Generator
 */
static inline uint32_t getIndex(const char *str, int tableSize)
{
    unsigned int h = 0;
    const unsigned char *p;
    for (p = (const unsigned char *) str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h % tableSize;
}

hTabItem *createNewItem()
{
    hTabItem *newItem;

    if ((newItem = calloc(1, sizeof(hTabItem))) == NULL) {
        return NULL;
    }

    newItem->dataType = -1;
    newItem->params = -1;
    newItem->paramPosition = -1;

    return newItem;
}

void insertHashTable(tTable *t, hTabItem *newItem)
{
    int index = getIndex(newItem->name, t->size);

    if (t->array[index] == NULL) {
        t->array[index] = newItem;
    } else {
        hTabItem *i;

        i = t->array[index];
        while (true) {
            if (i->next == NULL) {
                i->next = newItem;
                break;
            } else {
                i = i->next;
            }
        }
    }
}

hTabItem *searchItem(const tTable *t, const char *name)
{
    int index = getIndex(name, t->size);

    if (t->array[index] == NULL) {
        return NULL;
    } else if (strcmp(t->array[index]->name, name) == 0) {
        return t->array[index];
    } else {
        hTabItem *i;
        i = t->array[index];

        while (i != NULL) {
            if (strcmp(i->name, name) == 0) {
                return i;
            } else {
                i = i->next;
            }
        }
    }

    return NULL;
}
