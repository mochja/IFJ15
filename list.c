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

#include "list.h"

void listInit(tvarList *L) {
    L->First = NULL;
    L->Last = NULL;
}

void listDispose(tvarList *L) {
    tItemPtr tmp;
    tmp = L->First;

    while (tmp != NULL) {
        for (size_t i = 0; i < kv_size(tmp->data); ++i) {
            free(kv_A(tmp->data, i).id);
        }
        kv_destroy(tmp->data);
        L->First = L->First->next;
        free(tmp);
        tmp = L->First;
    }
}

void insertLast(tItemPtr item, tvarList *L) {
    item->next = NULL;
    item->prev = L->Last;

    if (L->Last != NULL) {
        L->Last->next = item;
    }
    else {
        L->First = item;
    }
    L->Last = item;
}

void deleteLast(tvarList *L) {
    tItemPtr tmp;
    tmp = L->Last;

    if (tmp != NULL) {
        for (size_t i = 0; i < kv_size(tmp->data); ++i) {
            free(kv_A(tmp->data, i).id);
        }
        kv_destroy(tmp->data);
        if (L->Last == L->First) {
            L->First = NULL;
        }
        L->Last = L->Last->prev;
        free(tmp);
        if (L->Last != NULL) {
            L->Last->next = NULL;
        }
    }
}

char *varSearch(tvarList *L, char *name) {
    tItemPtr tmp;
    tmp = L->Last;

    char *result = NULL;

    while (tmp != NULL) {
        int i = 0;
        while (i < kv_size(tmp->data)) {
            if (!strcmp(name, kv_A(tmp->data, i).id)) {
                result = kv_A(tmp->data, i).hid;
                return result;
            }
            i++;
        }
        tmp = tmp->prev;
    }

    return result;
}

char *paramSearch(tvarList *L, char *fid, char *name) {
    tItemPtr tmp;
    tmp = L->First;
    char *result = NULL;

    while (tmp != NULL) {
        if (!strcmp(tmp->functionId, fid)) {
            int i = 0;
            while (i < kv_size(tmp->data)) {
                if (!strcmp(name, kv_A(tmp->data, i).id)) {
                    result = kv_A(tmp->data, i).hid;
                    break;
                }
                i++;
            }
            return result;
        }
        tmp = tmp->next;
    }

    return result;
}

int get_var_offset(tvarList *L, char * name) {
    tItemPtr tmp;
    tmp = L->Last;

    int result = 0;

    while (tmp != NULL) {
        int i = 0;
        while (i < kv_size(tmp->data)) {
            if (!strcmp(name, kv_A(tmp->data, i).id)) {
                result = kv_A(tmp->data, i).offset;
                return result;
            }
            i++;
        }
        tmp = tmp->prev;
    }

    return result;
}

int get_param_offset(tvarList *L, char * fid, char *name){
    tItemPtr tmp;
    tmp = L->First;
    int result = 0;

    while (tmp != NULL) {
        if (!strcmp(tmp->functionId, fid)) {
            int i = 0;
            while (i < kv_size(tmp->data)) {
                if (!strcmp(name, kv_A(tmp->data, i).id)) {
                    result = kv_A(tmp->data, i).offset;
                    break;
                }
                i++;
            }
            return result;
        }
        tmp = tmp->next;
    }

    return result;
}
