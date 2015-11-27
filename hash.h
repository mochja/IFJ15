#ifndef HASH_H_
#define HASH_H_

#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#define MAX_HTSIZE 101

#define uint32_t unsigned int

struct hItem {
    bool isFree;
    char *key;
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
} tHItem;

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
