#ifndef LIST_H_
#define LIST_H_

#include <stdio.h>
#include <malloc.h>
#include <string.h>

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

#endif // LIST_H_
