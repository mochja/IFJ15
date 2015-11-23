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

    signed int Id;
    char* name;
    int dataType;
    int value;
    char* function;
}tHItem;

typedef struct hItem hTabItem;

typedef struct {
    int size;
    hTabItem *array[];
}tTable;

tTable * initHashTable(int size );
void insertHashTable(tTable *t, char *k);
void freeHashTable(tTable * t);

#endif  // HASH_H_
