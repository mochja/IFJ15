#include "hash.h"
/**
    HASH TABLE INITIALIZATION
**/
tTable * initHashTable(int size)
{
    tTable *t = NULL;   //initialization of table itself
    if((t =(tTable *)malloc( (sizeof(tTable)) + (sizeof(tHItem *) * size) )) == NULL)
    {
        printf("ERRk\n");
    }
    t->size = size;
    for (int i = 0; i < t->size; ++i)   //setting up startup values for correct
    {                                   //evaluation of conditions
        t->array[i] = NULL;
    }
    return t;                           //returning pointer to new table
}
/**
    HASH TABLE FREE
**/
void freeHashTable(tTable *t)
{
    static int x =0;
    ++x;
    if (t == NULL) return;              //check is table is initialized

    for (int i = 0; i < t->size; ++i)   //run through table to check all fields of array
    {
        hTabItem *freeItem;
        freeItem = t->array[i];
        while(true)
        {
            if (t->array[i] == NULL || t->array[i]->next == NULL)
            {
                break;
            }
            else
            {
                freeItem = t->array[i]->next;
                t->array[i]->next = freeItem->next;
                free(freeItem);
            }
        }
        free(t->array[i]);
    }
    free(t);
    return ;
}
/**
    Hash Function
**/
unsigned int getIndex(const char *str,int tableSize) {    //hash funkction return index value calculated for
          unsigned int h=0;                 //selected string
          const unsigned char *p;
          for(p=(const unsigned char*)str; *p!='\0'; p++)
              h = 65599*h + *p;
          return h % tableSize;
}
/**
    Insert into table
**/
void insertHashTable(tTable *t, char *k){
    static int counter = 0;
    int index = getIndex(k,t->size);
    ++counter;

    hTabItem *newItem;
    if( (newItem =(hTabItem*) malloc ( sizeof(hTabItem) ) ) == NULL)
    {
        fprintf(stderr, "Could not allocate new item \n");
    }
    newItem->next = NULL;
    newItem->key = k;
    newItem->name = k;
    newItem->value = counter;

    if (t->array[index] == NULL)
    {
        t->array[index] = newItem;
    }
    else
    {
        hTabItem *i;
        i = t->array[index];
        while(true)
        {
            if (i->next == NULL)
            {
                i->next = newItem;
                break;
            }
            else
            {
                i=i->next;
            }
        }
    }
    return;
}


int main()
{

    tTable *r = NULL;
    r = initHashTable(10);

    insertHashTable(r,"Martin");
    insertHashTable(r,"Martin");

    insertHashTable(r,"Martin");
    insertHashTable(r,"Martin");
    insertHashTable(r,"Martin");
    insertHashTable(r,"Martin");
    for (hTabItem *i = r->array[3]; i != NULL; i = i->next)
    {
        printf("1:%d \t%s\n",i->value,i->key );
    }


     //printf("1:%d \t%s\n",r->array[3]->value,r->array[3]->key );

    freeHashTable(r);

    return 0;
}
