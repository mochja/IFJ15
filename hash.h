#ifndef HASH_H_
#define HASH_H_

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#define MAX_HTSIZE 101

typedef struct hItem{
    bool isFree;
    char *key;
    struct hItem *next;

    char* name;
    int dataType;
    union
    {
        int i;
        double d;
        char *c;
    }value;
    char* function;

    bool isDefined;
    int params;
    int paramPosition;
}tHItem;

typedef struct hItem hTabItem;

typedef struct {
    int size;
    hTabItem *array[];
}tTable;

tTable * initHashTable(int size );
hTabItem * createNewItem();
void insertHashTable(tTable *t, hTabItem *newItem);
void freeHashTable(tTable * t);
//void printItem(tHItem *i);

#endif  // HASH_H_
