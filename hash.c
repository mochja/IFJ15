#include "hash.h"
/**
    HASH TABLE INITIALIZATION
**/
void initHashTable(tList t)
{
    if ((t = malloc (sizeof(struct hItem*)*MAX_HTSIZE)) == NULL)
    {
        printf("ERR\n");
    }
    for (int i = 0; i < MAX_HTSIZE; ++i)
    {
        if ((t[i]=malloc(sizeof(struct hItem))) == NULL)
        {
            printf("ERR\n");
        }
        t[i]->isFree=true;
        t[i]->next =NULL;
        t[i]->key =NULL;
        t[i]->d =NULL;
    }
    return ;
}
/**
    HASH TABLE FREE
**/
/*void freeHashTable(tList t)
{
    for (int i = 0; i < MAX_HTSIZE; ++i)
    {
        free(t[i]);
    }
    printf("CLEANED\n");
    return ;
}*/

/**
    Hash Function
**/
unsigned int hash_function(const char *str) {
          unsigned int h=0;
          const unsigned char *p;
          for(p=(const unsigned char*)str; *p!='\0'; p++)
              h = 65599*h + *p;
          return h % MAX_HTSIZE;
}

void tableInsert(tList t, char *k){
    int index = hash_function(k);
    if (t[index]->isFree)
    {
        t[index]->isFree = false;
        t[index]->key = k;
        if ((t[index]->d = malloc(sizeof(struct data))) == NULL)
        {
            printf("ERR\n");
        }
    }
    else
    {
        for (tHItem *i = t[index]; i != NULL ; i=i->next)
        {
            if (i->next == NULL)
            {
                if((i->next = malloc(sizeof(struct tHItem *))) == NULL) printf("ERR\n");
                if((i->next->d = malloc(sizeof(struct data))) == NULL) printf("ERR\n");

            }
        }
    }

}
int main()
{
    tList t;
    initHashTable(t);
    //freeHashTable(t);
    printf("lel\n");
    return 0;
}
