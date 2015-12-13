#ifndef IAL_H_
#define IAL_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>

#define length(x) strlen(x)

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

char* substr(const char *str, int pos, int count);
char* concat(const char *str1, const char *str2);
int find(const char*, const char*);
char* sort(const char*);

#endif  // IAL_H_
