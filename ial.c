#include "ial.h"

/*
 * Returns a substring [pos, pos+count). If the requested substring
 * extends past the end of the string, or if count == npos, the returned substring is [pos, size()).
 */
char* substr(const char *str, int pos, int count)
{
    char *result_str;
    size_t str_len = length(str);

    if (pos < 0 || count < 0 || pos > str_len) {
        // TODO: proper error handling
        exit(1);
    }

    size_t result_len = (count > str_len - pos) ? str_len - pos : (size_t) count;
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

    size_t str1_len = length(str1);
    size_t str2_len = length(str2);

    result_str = malloc((str1_len + str2_len + 1) * sizeof(char));

    if (result_str == NULL) {
        // TODO: proper error handling
        exit(1);
    }

    memcpy(result_str, str1, str1_len);
    memcpy(result_str + str1_len, str2, str2_len + 1);

    return result_str;
}

char *sort(const char *s) {

    char *result_str = malloc((strlen(s) + 1) * sizeof(char));
    strcpy(result_str, s);

    size_t gap = strlen(s) / 2;
    size_t i, j;
    char tmp;

    while (gap > 0) {
        for (i = 0; i < strlen(s)-gap; i++) {
            j = i+gap;
            tmp = result_str[j];
            while (result_str[j - gap] > tmp && j >= gap) {
                result_str[j] = result_str[j - gap];
                j -= gap;
            }
            result_str[j] = tmp;
        }
        gap = (gap == 2) ? 1 : (size_t) (gap / 2.2);
    }

    return result_str;
}

#define ALBHABETH_LENGTH 256

static inline void do_delta(int delta[], const char *search, int searchLen) {
    int i;

    for (i = 0; i < ALBHABETH_LENGTH; i++) {
        delta[i] = searchLen;
    }

    for (i = 0; i <= searchLen - 1; i++) {
        delta[(int) search[i]] = searchLen - i - 1;
    }
}

int find(const char *s, const char *search) {
    int i, sLen = (int) strlen(s), searchLen = (int) strlen(search);
    if (searchLen == 0) return 0;

    int delta[ALBHABETH_LENGTH];
    do_delta(delta, search, searchLen);

    i = searchLen - 1;
    while (i < sLen) {
        int j = searchLen - 1;

        while (j >= 0 && s[i] == search[j]) {
            j--;
            i--;
        }

        if (j < 0) return i + 1;

        i += delta[(int) s[i]];
    }
    // returns -1 when search is not in s
    return -1;
}



/*
 * HASH TABLE
 */
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
        while (t->array[i] != NULL) {
            hTabItem *next = t->array[i]->next;
            if (next != NULL) {
                free(t->array[i]->name);
                free(t->array[i]);
                t->array[i] = next;
                continue;
            } else if (t->array != NULL) {
                free(t->array[i]->name);
                free(t->array[i]);
            }
            break;
        }
    }
    free(t);
}

/*!
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
    newItem->sVal = NULL;
    newItem->name = NULL;

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
