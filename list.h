#ifndef LIST_H_
#define LIST_H_

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "globals.h"

typedef struct Data
{
	char * id;
	char * hid;
}tdata;

typedef struct tItem{
	struct tItem * next;
	struct tItem * prev;
	tdata *data;
	char * functionId;
	int number;
}*tItemPtr;


typedef struct{
	tItemPtr First;
	tItemPtr Last;
}tvarList;


void listInit(tvarList *L);
void listDispose(tvarList *L);
void insertLast(tItemPtr item, tvarList *L);
void deleteLast(tvarList *L);
char * varSearch(tvarList *L, char * name);
char * paramSearch(tvarList *L, char * fid, char *name);

static inline result_t init_data_var(tdata *dest, const char *id, const char *hid) {

    dest->id = malloc((strlen(id) + 1) * sizeof(char));
    dest->hid = malloc((strlen(hid) + 1) * sizeof(char));

    strcpy(dest->id, id);
    strcpy(dest->hid, hid);

    return EOK;
}

#endif // LIST_H_
