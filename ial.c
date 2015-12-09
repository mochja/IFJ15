#include "ial.h"

/*
 * Returns a substring [pos, pos+count). If the requested substring
 * extends past the end of the string, or if count == npos, the returned substring is [pos, size()).
 */
char* substr(const char *str, int pos, int count)
{
    char *result_str;
    const int str_len = length(str);

    if (pos < 0 || count < 0 || pos > str_len) {
        // TODO: proper error handling
        exit(1);
    }

    const int result_len = (count > str_len - pos) ? str_len - pos : count;
    result_str = malloc((result_len + 1) * sizeof(char));

    if (result_str == NULL) {
        // TODO: proper error handling
        exit(1);
    }

    if (pos < str_len) {
        memcpy(result_str, str + pos, result_len);
    }

    result_str[result_len] = '\0';

    return result_str;
}

char* concat(const char *str1 , const char *str2)
{
    char *result_str;

    const int str1_len = length(str1);
    const int str2_len = length(str2);

    result_str = malloc((str1_len + str2_len + 1) * sizeof(char));

    if (result_str == NULL) {
        // TODO: proper error handling
        exit(1);
    }

    memcpy(result_str, str1, str1_len);
    memcpy(result_str + str1_len, str2, str2_len + 1);

    return result_str;
}


/* HASH TABLE */

tTable *initHashTable(int size)
{
    tTable *t;

    if ((t = calloc(1, sizeof(tTable) + sizeof(struct hItem) * size)) == NULL) {
        return NULL;
    }

    t->size = size;

    return t;
}

void freeHashTable(tTable *t)
{
    if (t == NULL) {
        return;
    }

    for (int i = 0; i < t->size; ++i) {
        hTabItem *freeItem;

        freeItem = t->array[i];
        while (true) {
            if (t->array[i] == NULL || t->array[i]->next == NULL) {
                break;
            } else {
                freeItem = t->array[i]->next;
                t->array[i]->next = freeItem->next;
                free(freeItem);
            }
        }
        free(t->array[i]);
    }
    free(t);
}

/*
 * Hash Index Generator
 */
static inline uint32_t getIndex(const char *str, int tableSize)
{
    unsigned int h = 0;
    const unsigned char *p;
    for (p = (const unsigned char *) str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h % tableSize;
}

hTabItem *createNewItem()
{
    hTabItem *newItem;

    if ((newItem = calloc(1, sizeof(hTabItem))) == NULL) {
        return NULL;
    }

    newItem->dataType = -1;
    newItem->params = -1;
    newItem->paramPosition = -1;

    return newItem;
}

void insertHashTable(tTable *t, hTabItem *newItem)
{
    int index = getIndex(newItem->name, t->size);

    if (t->array[index] == NULL) {
        t->array[index] = newItem;
    } else {
        hTabItem *i;

        i = t->array[index];
        while (true) {
            if (i->next == NULL) {
                i->next = newItem;
                break;
            } else {
                i = i->next;
            }
        }
    }
}

hTabItem *searchItem(const tTable *t, const char *name)
{
    int index = getIndex(name, t->size);

    if (t->array[index] == NULL) {
        return NULL;
    } else if (strcmp(t->array[index]->name, name) == 0) {
        return t->array[index];
    } else {
        hTabItem *i;
        i = t->array[index];

        while (i != NULL) {
            if (strcmp(i->name, name) == 0) {
                return i;
            } else {
                i = i->next;
            }
        }
    }

    return NULL;
}
