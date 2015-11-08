#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#define MAX_HTSIZE 101

typedef struct data{
    signed int Id;
    char* name;
    int dataType;
    int value;
    char* function;
}tData;

 typedef struct hItem{
    bool isFree;
    char *key;
    struct data *d;
    struct hItem* next;
} tHItem;

typedef tHItem *tList[MAX_HTSIZE];

void initHashTable(tHItem **t);



