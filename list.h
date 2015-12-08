#ifndef LIST_H_
#define LIST_H_

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "globals.h"
#include "kvec.h"

typedef struct {
    char *id;
    char hid[64];
    int offset;
} tData;

typedef struct tItem {
    struct tItem *next;
    struct tItem *prev;
    kvec_t(tData) data;
    char *functionId;
} *tItemPtr;

typedef struct {
    tItemPtr First;
    tItemPtr Last;
} tvarList;

void listInit(tvarList *L);
void listDispose(tvarList *L);
void insertLast(tItemPtr item, tvarList *L);
void deleteLast(tvarList *L);
char * varSearch(tvarList *L, char * name);
char * paramSearch(tvarList *L, char * fid, char *name);
int get_var_offset(tvarList *L, char * name);
int get_param_offset(tvarList *L, char * fid, char *name);


static inline result_t init_data_var(tData *dest, const char *id, const char *hid,const int offset) {

    dest->id = malloc((strlen(id) + 1) * sizeof(char));

    strcpy(dest->id, id);
    strcpy(dest->hid, hid);
    dest->offset=offset;

    return EOK;
}

static inline void item_append_data(tItemPtr item, tData data) {
    kv_push(tData, item->data, data);
}

static inline result_t list_foo_bar(tItemPtr item, const char *find) {

    int i = 0;
    while (i < kv_size(item->data)) {
        if (!strcmp(find, kv_A(item->data, i).id)) {
            return ESEM;
        }
        i++;
    }

    return EOK;
}

#endif // LIST_H_
