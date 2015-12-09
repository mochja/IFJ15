#ifndef IAL_H_
#define IAL_H_

#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#define MAX_HTSIZE 101

#define uint32_t unsigned int

#define length(x) strlen(x)

char* substr(const char *str, int pos, int count);
char* concat(const char *str1 , const char *str2);
int find(char*, char*);
char* sort(char*);

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

#endif  // IAL_H_
